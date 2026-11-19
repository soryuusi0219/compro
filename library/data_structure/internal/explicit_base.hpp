// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/explicit_base.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "reversible_monoid.hpp"
#include "tree_arena.hpp"

namespace soryuusi::internal {

template<class Key, class Balance, class KeyAction>
struct ExplicitExtra : Balance {
  struct NoKeyState {};
  Key key;
  [[no_unique_address]] std::conditional_t<KeyAction::enabled,typename KeyAction::State,NoKeyState> key_action;
  explicit ExplicitExtra(const Key& k) : key(k){}
};
template<class Key, class S, auto op, auto e, class Action, class Balance, class KeyAction = NoTreeAction>
struct ExplicitMonoid : ReversibleMonoid<S,op,e,Action,true,false,ExplicitExtra<Key,Balance,KeyAction>> {
  using Extra = ExplicitExtra<Key,Balance,KeyAction>;
  using Base = ReversibleMonoid<S,op,e,Action,true,false,Extra>;
  using Node = typename Base::Node;
  using ValueAction = Action;
  using KeyUpdate = KeyAction;
  using key_action_type = typename KeyAction::value_type;
  static constexpr bool has_key_action = KeyAction::enabled;
  static const Key& key(const Node* p){return p->extra.key;}
  static S combine(const S& a, const S& b){return op(a,b);}
  static void map_keys(Node* p, const key_action_type& f) requires has_key_action {
    if(!p) return;
    p->extra.key = KeyAction::map(f,p->extra.key);
    auto& lazy = p->extra.key_action.lazy;
    lazy = KeyAction::compose(f,lazy);
  }
  static void apply_keys(Node* p, const key_action_type& f, bool decreasing) requires has_key_action {
    map_keys(p,f);
    if(decreasing) Base::toggle(p);
  }
  static void push(Node* p){
    if constexpr(has_key_action){
      auto& lazy = p->extra.key_action.lazy;
      if(lazy != KeyAction::identity()){
        map_keys(p->lp,lazy); map_keys(p->rp,lazy);
        lazy = KeyAction::identity();
      }
    }
    Base::push(p); // Propagate reversal separately from the key mapping.
  }
};
template<class Key, class Ops, template<class,class> class Backend, bool Multi, class Compare>
struct ExplicitStorage : TreeArena<Ops,Backend> {
  Compare compare;
  explicit ExplicitStorage(Compare comp) : compare(std::move(comp)){}
};
template<class Key, class Ops, template<class,class> class Backend, bool Multi, class Compare> class ExplicitArena;
template<class Key, class Ops, template<class,class> class Backend, class Balance, class Compare> class SortableSequence;

template<class Key, class Ops, template<class,class> class Backend, bool Multi, class Compare>
class ExplicitSet {
  friend class ExplicitArena<Key,Ops,Backend,Multi,Compare>;
  template<class, class, template<class,class> class, class, class> friend class SortableSequence;
  using Node = typename Ops::Node;
  using S = typename Ops::value_type;
  using Arena = TreeArena<Ops,Backend>;
  using Storage = ExplicitStorage<Key,Ops,Backend,Multi,Compare>;
  std::shared_ptr<Storage> arena;
  Node* root = nullptr;
  explicit ExplicitSet(std::shared_ptr<Storage> owner, Node* p = nullptr) : arena(std::move(owner)), root(p){}
  bool less(const Key& a, const Key& b) const {return arena->compare(a,b);}
  int bound(Node*& tree, const Key& key, bool upper){
    int passed = 0, last_rank = 0;
    Node* last = nullptr;
    for(Node* p = tree; p;){
      Ops::push(p);
      last = p; last_rank = passed+Ops::size(p->lp);
      bool go_right = upper ? !less(key,Ops::key(p)) : less(Ops::key(p),key);
      if(go_right){passed = last_rank+1; p = p->rp;}
      else p = p->lp;
    }
    // Splaying the last visited node is essential for amortized search bounds.
    if(last){
      if constexpr(requires {arena->engine.access_node(tree,last);}) arena->engine.access_node(tree,last);
      else arena->engine.access(tree,last_rank);
    }
    return passed;
  }
  Node* make_node(const Key& key, const S& value){return arena->create(value,typename Ops::Extra(key));}
  Node* build_sorted(const std::vector<std::pair<Key,S>>& entries){
    return arena->engine.build(entries.size(),[&](int i){return make_node(entries[i].first,entries[i].second);});
  }
  template<class Entry, class GetKey>
  void normalize(std::vector<Entry>& entries, GetKey key){
    std::stable_sort(entries.begin(),entries.end(),[&](const Entry& a,const Entry& b){return less(key(a),key(b));});
    if constexpr(!Multi){
      auto end = std::unique(entries.begin(),entries.end(),[&](const Entry& a,const Entry& b){
        return !less(key(a),key(b)) && !less(key(b),key(a));
      });
      entries.erase(end,entries.end());
    }
  }
  template<class Function> auto range(int l, int r, Function function){
    assert(0 <= l && l <= r && r <= size());
    if(l == r){Node* empty = nullptr; return function(empty);}
    auto [a,bc] = arena->engine.split(root,l);
    auto [b,c] = arena->engine.split(bc,r-l);
    auto result = function(b);
    root = arena->engine.join(arena->engine.join(a,b),c);
    return result;
  }
  void release_tree(Node* p){
    if(!p) return;
    if(!p->lp && !p->rp){arena->release(p); return;}
    std::vector<Node*> stack;
    if(p) stack.push_back(p);
    while(!stack.empty()){
      Node* q = stack.back(); stack.pop_back();
      if(q->lp) stack.push_back(q->lp);
      if(q->rp) stack.push_back(q->rp);
      arena->release(q);
    }
  }
  std::tuple<Node*,Node*,Node*> partition(Node* p, const Key& key){
    int l = bound(p,key,false), r = bound(p,key,true);
    auto [a,bc] = arena->engine.split(p,l);
    auto [b,c] = arena->engine.split(bc,r-l);
    return {a,b,c};
  }
  Node* unite(Node* a, Node* b){
    if(!a || !b) return a ? a : b;
    Node*& smaller = Ops::size(a) <= Ops::size(b) ? a : b;
    Key pivot = Ops::key(arena->engine.access(smaller,Ops::size(smaller)/2));
    auto [al,ae,ar] = partition(a,pivot);
    auto [bl,be,br] = partition(b,pivot);
    Node* middle;
    if constexpr(Multi) middle = arena->engine.join(ae,be);
    else{
      middle = ae ? ae : be;
      if(ae) release_tree(be); // Destination value wins on equal keys.
    }
    Node* left = unite(al,bl);
    Node* right = unite(ar,br);
    return arena->engine.join(arena->engine.join(left,middle),right);
  }
public:
  using key_type = Key;
  using value_type = S;
  using NodeRef = typename Arena::NodeRef;
  static constexpr bool allows_duplicates = Multi;
  ExplicitSet(const ExplicitSet&) = delete;
  ExplicitSet& operator=(const ExplicitSet&) = delete;
  ExplicitSet(ExplicitSet&& other) noexcept : arena(std::move(other.arena)), root(std::exchange(other.root,nullptr)){}
  ExplicitSet& operator=(ExplicitSet&& other) noexcept {
    if(this != &other){arena = std::move(other.arena); root = std::exchange(other.root,nullptr);}
    return *this;
  }
  int size() const {return Ops::size(root);}
  bool empty() const {return !root;}
  std::size_t allocated_nodes() const {return arena ? arena->pool.size() : 0;}
  int lower_bound(const Key& key){return bound(root,key,false);}
  int upper_bound(const Key& key){return bound(root,key,true);}
  int count(const Key& key){int l = lower_bound(key); int r = upper_bound(key); return r-l;}
  bool contains(const Key& key){return count(key) != 0;}
  bool insert(const Key& key, const S& value){
    int pos = Multi ? upper_bound(key) : lower_bound(key);
    if constexpr(!Multi){
      if(pos < size()){
        Node* p = arena->engine.access(root,pos);
        if(!less(key,Ops::key(p))) return false;
      }
    }
    auto [a,b] = arena->engine.split(root,pos);
    root = arena->engine.join(arena->engine.join(a,make_node(key,value)),b);
    return true;
  }
  bool insert(const Key& key) requires std::is_constructible_v<S,Key> {return insert(key,S(key));}
  std::pair<Key,S> kth(int i){assert(0 <= i && i < size()); auto p = arena->engine.access(root,i); return {Ops::key(p),p->value};}
  [[nodiscard]] NodeRef ref_at(int i){
    assert(0 <= i && i < size());
    return arena->reference(arena,arena->engine.access(root,i));
  }
  int index_of(const NodeRef& ref){return arena ? arena->index_of(root,ref) : -1;}
  void set_at(int i, S value){
    assert(0 <= i && i < size());
    Node* p = arena->engine.access(root,i); p->value = std::move(value);
    for(;p;p = p->pp) Ops::pull(p);
  }
  int erase_range(int l, int r){
    return range(l,r,[&](Node*& p){release_tree(p); p = nullptr; return r-l;});
  }
  void erase_at(int i){assert(0 <= i && i < size()); erase_range(i,i+1);}
  bool erase(const Key& key){int l = lower_bound(key), r = upper_bound(key); if(l == r) return false; erase_at(l); return true;}
  int erase_all(const Key& key){int l = lower_bound(key), r = upper_bound(key); return erase_range(l,r);}
  S prod(int l, int r){return range(l,r,[](Node* p){return p ? p->lsum : Ops::identity();});}
  S prodrev(int l, int r){return range(l,r,[](Node* p){return p ? p->rsum : Ops::identity();});}
  S prod_all() const {return root ? root->lsum : Ops::identity();}
  S prodrev_all() const {return root ? root->rsum : Ops::identity();}
  S prod_key(const Key& low, const Key& high){assert(!less(high,low)); int l = lower_bound(low), r = lower_bound(high); return prod(l,r);}
  void apply(int l, int r, typename Ops::action_type f) requires Ops::has_action {
    range(l,r,[&](Node* p){Ops::apply(p,f); return 0;});
  }
  void apply_key(const Key& low, const Key& high, typename Ops::action_type f) requires Ops::has_action {
    assert(!less(high,low)); int l = lower_bound(low), r = lower_bound(high); apply(l,r,f);
  }
  void apply_keys(int l, int r, typename Ops::key_action_type f, bool decreasing = false) requires Ops::has_key_action {
    assert(0 <= l && l <= r && r <= size());
    if(l == r) return;
    auto& engine = arena->engine;
    auto [a,bc] = engine.split(root,l);
    auto [b,c] = engine.split(bc,r-l);
    Ops::apply_keys(b,f,decreasing);
    auto ordered = [&](Node*& left, Node*& right){
      if(!left || !right) return true;
      const Key& x = Ops::key(engine.access(left,Ops::size(left)-1));
      const Key& y = Ops::key(engine.access(right,0));
      if constexpr(Multi) return !less(y,x);
      else return less(x,y);
    };
    if(ordered(a,b) && ordered(b,c)) root = engine.join(engine.join(a,b),c);
    else root = unite(engine.join(a,c),b);
  }
  void apply_keys_by_key(const Key& low, const Key& high, typename Ops::key_action_type f,
                         bool decreasing = false) requires Ops::has_key_action {
    assert(!less(high,low));
    int l = lower_bound(low), r = lower_bound(high);
    apply_keys(l,r,f,decreasing);
  }
  [[nodiscard]] std::pair<ExplicitSet,ExplicitSet> split(int k){
    assert(0 <= k && k <= size());
    auto [a,b] = arena->engine.split(root,k); root = nullptr;
    return {ExplicitSet(arena,a),ExplicitSet(arena,b)};
  }
  [[nodiscard]] std::pair<ExplicitSet,ExplicitSet> split_key(const Key& key){return split(lower_bound(key));}
  void meld(ExplicitSet&& other){
    assert(this != &other && arena && other.arena);
    if(!other.root) return;
    Node* incoming = other.root;
    if(arena != other.arena){
      std::vector<Node*> source;
      source.reserve(other.size());
      Arena::visit_in_order(other.root,[&](Node* p){source.push_back(p);});
      normalize(source,[](const Node* p) -> const Key& {return Ops::key(p);});
      std::shared_ptr<Arena> destination = arena;
      incoming = arena->engine.build(source.size(),[&](int i){
        Node* old = source[i];
        Node* p = make_node(Ops::key(old),old->value);
        other.arena->transfer_reference(old,destination,p);
        return p;
      });
    }
    root = unite(root,incoming); other.root = nullptr;
  }
  std::vector<std::pair<Key,S>> to_vector(){
    std::vector<std::pair<Key,S>> result;
    result.reserve(size());
    Arena::visit_in_order(root,[&](Node* p){result.emplace_back(Ops::key(p),p->value);});
    return result;
  }
  [[nodiscard]] ExplicitSet clone(){ExplicitSet copy(arena); copy.root = copy.build_sorted(to_vector()); return copy;}
  void clear(){root = nullptr;}
};

template<class Key, class Ops, template<class,class> class Backend, bool Multi, class Compare>
class ExplicitArena {
  using Storage = ExplicitStorage<Key,Ops,Backend,Multi,Compare>;
  std::shared_ptr<Storage> arena;
public:
  using Set = ExplicitSet<Key,Ops,Backend,Multi,Compare>;
  using NodeRef = typename Set::NodeRef;
  explicit ExplicitArena(Compare compare = Compare()) : arena(std::make_shared<Storage>(std::move(compare))){}
  ExplicitArena(const ExplicitArena&) = delete;
  ExplicitArena& operator=(const ExplicitArena&) = delete;
  ExplicitArena(ExplicitArena&&) noexcept = default;
  ExplicitArena& operator=(ExplicitArena&&) noexcept = default;
  [[nodiscard]] Set new_set(){assert(arena); return Set(arena);}
  [[nodiscard]] Set new_set(std::vector<std::pair<Key,typename Ops::value_type>> entries){
    auto result = new_set();
    result.normalize(entries,[](const auto& entry) -> const Key& {return entry.first;});
    result.root = result.build_sorted(entries);
    return result;
  }
  std::size_t allocated_nodes() const {return arena ? arena->pool.size() : 0;}
};

}  // namespace soryuusi::internal
