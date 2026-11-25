// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/sortable_base.hpp
#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
#include "binary_tree_ops.hpp"
#include "explicit_base.hpp"
#include "tree_arena.hpp"

namespace soryuusi::internal {

// Only the root of an explicit tree needs an up-to-date outer owner.
template<class Balance> struct SortableInnerState : Balance {void* block = nullptr;};
template<class Action, bool = Action::enabled> struct SortableKeyTag {};
template<class Action> struct SortableKeyTag<Action,true> : Action::State {bool decreasing = false;};

template<class InnerOps, class Balance>
struct SortableBlockOps {
  using Inner = typename InnerOps::Node;
  using S = typename InnerOps::value_type;
  using ValueAction = typename InnerOps::ValueAction;
  using KeyAction = typename InnerOps::KeyUpdate;

  struct Node : Balance {
    Node* pp = nullptr;
    Node* lp = nullptr;
    Node* rp = nullptr;
    Inner* inner;
    S lsum, rsum;
    int size;
    bool descending, reversed = false;
    [[no_unique_address]] typename ValueAction::State action;
    [[no_unique_address]] SortableKeyTag<KeyAction> key_action;
    explicit Node(Inner* block, bool descending = false)
        : inner(block), lsum(descending ? block->rsum : block->lsum),
          rsum(descending ? block->lsum : block->rsum),
          size(InnerOps::size(block)), descending(descending){}
  };

  static auto& priority(Node* p) requires requires {p->priority;} {return p->priority;}
  static int local_size(const Node* p){return InnerOps::size(p->inner);}
  static int size(const Node* p){return p ? p->size : 0;}
  static void pull(Node* p){
    p->size = size(p->lp)+local_size(p)+size(p->rp);
    p->lsum = p->descending ? p->inner->rsum : p->inner->lsum;
    p->rsum = p->descending ? p->inner->lsum : p->inner->rsum;
    if(p->lp){p->lsum = InnerOps::combine(p->lp->lsum,p->lsum); p->rsum = InnerOps::combine(p->rsum,p->lp->rsum);}
    if(p->rp){p->lsum = InnerOps::combine(p->lsum,p->rp->lsum); p->rsum = InnerOps::combine(p->rp->rsum,p->rsum);}
  }
  static void toggle(Node* p){
    if(!p) return;
    std::swap(p->lp,p->rp);
    std::swap(p->lsum,p->rsum);
    p->descending ^= true;
    p->reversed ^= true;
  }
  static void apply(Node* p, const typename ValueAction::value_type& f) requires ValueAction::enabled {
    if(!p) return;
    InnerOps::apply(p->inner,f);
    p->lsum = ValueAction::map(f,p->lsum);
    p->rsum = ValueAction::map(f,p->rsum);
    p->action.lazy = ValueAction::compose(f,p->action.lazy);
  }
  static void apply_keys(Node* p, const typename KeyAction::value_type& f, bool decreasing) requires KeyAction::enabled {
    if(!p) return;
    InnerOps::apply_keys(p->inner,f,decreasing);
    p->descending ^= decreasing; // Canonical key order changed; sequence order did not.
    p->key_action.lazy = KeyAction::compose(f,p->key_action.lazy);
    p->key_action.decreasing ^= decreasing;
  }
  static void push(Node* p){
    if constexpr(ValueAction::enabled){
      auto& f = p->action.lazy;
      if(f != ValueAction::identity()){
        apply(p->lp,f); apply(p->rp,f);
        f = ValueAction::identity();
      }
    }
    if constexpr(KeyAction::enabled){
      auto& tag = p->key_action;
      if(tag.decreasing || tag.lazy != KeyAction::identity()){
        apply_keys(p->lp,tag.lazy,tag.decreasing); apply_keys(p->rp,tag.lazy,tag.decreasing);
        tag.lazy = KeyAction::identity(); tag.decreasing = false;
      }
    }
    if(p->reversed){toggle(p->lp); toggle(p->rp); p->reversed = false;}
  }
};

template<class Key, class Ops, template<class,class> class Backend, class Balance, class Compare> class SortableArena;

template<class Key, class Ops, template<class,class> class Backend, class Balance, class Compare>
class SortableSequence {
  friend class SortableArena<Key,Ops,Backend,Balance,Compare>;
  using S = typename Ops::value_type;
  using Inner = typename Ops::Node;
  using Blocks = SortableBlockOps<Ops,Balance>;
  using Node = typename Blocks::Node;
  using Outer = TreeArena<Blocks,Backend>;
  using Links = BinaryTreeOps<Node,Blocks>;
  static constexpr int small_sort_limit = 16;
  static constexpr int bulk_sort_threshold = 128;
  struct Storage {
    ExplicitSet<Key,Ops,Backend,true,Compare> entries;
    Outer blocks;
    explicit Storage(Compare compare)
        : entries(ExplicitArena<Key,Ops,Backend,true,Compare>(std::move(compare)).new_set()){}
  };
  std::shared_ptr<Storage> arena;
  Node* root = nullptr;
  explicit SortableSequence(std::shared_ptr<Storage> arena, Node* p = nullptr)
      : arena(std::move(arena)), root(p){}

  static void attach(Node* block, Inner* inner){
    block->inner = inner;
    inner->extra.block = block;
  }
  Node* make_block(Inner* inner, bool descending = false){
    Node* p = arena->blocks.create(inner,descending);
    attach(p,inner);
    return p;
  }
  Node* make_element(const Key& key, const S& value){
    return make_block(arena->entries.make_node(key,value));
  }
  std::pair<Node*,Node*> cut_block(Node* block, int offset){
    int length = Blocks::local_size(block);
    auto [first,second] = arena->entries.arena->engine.split(block->inner,block->descending ? length-offset : offset);
    if(block->descending) std::swap(first,second);
    attach(block,first);
    return {block,make_block(second,block->descending)};
  }
  std::pair<Node*,Node*> split_nodes(Node* tree, int k){
    auto& outer = arena->blocks.engine;
    auto cut = [&](Node* block,int offset){return cut_block(block,offset);};
    if constexpr(requires {outer.split_weighted(tree,k,cut);}) return outer.split_weighted(tree,k,cut);
    if(k == 0) return {nullptr,tree};
    if(k == Blocks::size(tree)) return {tree,nullptr};
    auto [p,offset] = Links::locate(tree,k);
    if(offset == 0) return outer.split(tree,k);
    const int length = Blocks::local_size(p);
    auto [a,bc] = outer.split(tree,k-offset);
    auto [block,c] = outer.split(bc,length);
    Blocks::push(block);
    auto [first,second] = cut(block,offset);
    Blocks::pull(first);
    return {outer.join(a,first),outer.join(second,c)};
  }
  template<class Function> auto range(int l, int r, Function function){
    assert(0 <= l && l <= r && r <= size());
    if(l == r){Node* empty = nullptr; return function(empty);}
    auto [a,bc] = split_nodes(root,l);
    auto [b,c] = split_nodes(bc,r-l);
    auto result = function(b);
    auto& engine = arena->blocks.engine;
    root = engine.join(engine.join(a,b),c);
    return result;
  }
  std::pair<Node*,Inner*> access(int i){
    assert(0 <= i && i < size());
    auto [block,offset] = Links::locate(root,i);
    auto& engine = arena->blocks.engine;
    if constexpr(requires {engine.access_pushed(root,block);}) engine.access_pushed(root,block);
    else engine.access_node(root,block);
    Inner* node = arena->entries.arena->engine.access(block->inner,block->descending ? Blocks::local_size(block)-1-offset : offset);
    attach(block,block->inner);
    return {block,node};
  }
  Inner* meld(Inner* a, Inner* b){
    if(!a || !b) return a ? a : b;
    auto& entries = arena->entries;
    if(auto* joined = entries.join_ordered(a,b)) return joined;
    return entries.unite(a,b);
  }
  Inner* sort_small(Node* tree){
    std::array<Inner*,small_sort_limit> nodes;
    int count = 0;
    auto collect_inner = [&](auto&& self,Inner* p) -> void {
      if(!p) return;
      Ops::push(p);
      self(self,p->lp);
      nodes[count++] = p;
      self(self,p->rp);
    };
    auto collect = [&](auto&& self,Node* p) -> void {
      if(!p) return;
      Blocks::push(p);
      self(self,p->lp);
      collect_inner(collect_inner,p->inner);
      self(self,p->rp);
      if(p != tree) arena->blocks.release(p);
    };
    collect(collect,tree);
    auto& entries = arena->entries;
    for(int i = 1; i < count; ++i){
      Inner* p = nodes[i];
      int j = i;
      while(j && entries.less(Ops::key(p),Ops::key(nodes[j-1]))){
        nodes[j] = nodes[j-1];
        --j;
      }
      nodes[j] = p;
    }
    for(int i = 0; i < count; ++i) nodes[i]->pp = nodes[i]->lp = nodes[i]->rp = nullptr;
    return entries.arena->engine.build(count,[&](int i){return nodes[i];});
  }
  Inner* sort_many(std::vector<Inner*>& blocks, int length){
    if(blocks.size() >= static_cast<std::size_t>(1+(length-1)/4)){
      // Small blocks are cheaper to sort together and rebuild in linear time.
      std::vector<Inner*> elements;
      if(blocks.size() == static_cast<std::size_t>(length)) elements.swap(blocks);
      else {
        elements.reserve(length);
        for(auto* block : blocks){
          if(Ops::size(block) == 1) elements.push_back(block);
          else TreeArena<Ops,Backend>::visit_in_order(block,[&](Inner* p){elements.push_back(p);});
        }
      }
      auto& entries = arena->entries;
      std::stable_sort(elements.begin(),elements.end(),[&](Inner* a,Inner* b){return entries.less(Ops::key(a),Ops::key(b));});
      for(auto* p : elements){Ops::push(p); p->pp = p->lp = p->rp = nullptr;}
      return entries.arena->engine.build(elements.size(),[&](int i){return elements[i];});
    }
    // Meld groups of equal block count, then combine them in sequence order.
    std::array<Inner*,std::numeric_limits<int>::digits> batches{};
    for(auto* part : blocks){
      int i = 0;
      while(batches[i]){
        part = meld(batches[i],part);
        batches[i++] = nullptr;
      }
      batches[i] = part;
    }
    Inner* result = nullptr;
    for(auto* part : batches) if(part) result = meld(part,result);
    return result;
  }
  template<bool Reverse> S fold_block(Node* block, int l, int r){
    int n = Blocks::local_size(block);
    bool backward = Reverse != block->descending;
    if(l == 0 && r == n) return backward ? block->inner->rsum : block->inner->lsum;
    if(block->descending){int start = n-r; r = n-l; l = start;}
    auto& storage = *arena->entries.arena;
    auto result = backward ? storage.template prod<true>(block->inner,l,r)
                           : storage.template prod<false>(block->inner,l,r);
    attach(block,block->inner);
    return result;
  }
  template<bool Reverse> S fold(int l, int r){
    assert(0 <= l && l <= r && r <= size());
    if(l == r) return Ops::identity();
    auto local = [&](Node* p,int from,int to){return fold_block<Reverse>(p,from,to);};
    return arena->blocks.engine.template fold_range<Reverse>(root,l,r,local,Ops::combine);
  }
  template<class Function> static void visit_elements(Node* tree, Function visit){
    Outer::visit_in_order(tree,[&](Node* p){
      TreeArena<Ops,Backend>::visit_in_order(p->inner,visit,p->descending);
    });
  }
  Node* copy_from(SortableSequence& source, bool transfer){
    auto& entries = arena->entries;
    auto copy = [&](Inner* node){
      Inner* p = entries.make_node(Ops::key(node),node->value);
      if(transfer) source.arena->entries.arena->transfer_reference(node,entries.arena,p);
      return p;
    };
    std::vector<Node*> blocks;
    std::vector<Inner*> elements;
    Outer::visit_in_order(source.root,[&](Node* block){
      Inner* inner = block->inner;
      if(Ops::size(inner) == 1){
        blocks.push_back(make_block(copy(inner)));
        return;
      }
      elements.clear();
      TreeArena<Ops,Backend>::visit_in_order(inner,[&](Inner* p){elements.push_back(p);});
      auto less = [&](Inner* a,Inner* b){return entries.less(Ops::key(a),Ops::key(b));};
      if(!transfer || std::is_sorted(elements.begin(),elements.end(),less)){
        auto* subtree = entries.arena->engine.build(elements.size(),[&](int i){return copy(elements[i]);});
        blocks.push_back(make_block(subtree,block->descending));
      } else {
        if(block->descending) std::reverse(elements.begin(),elements.end());
        for(auto* p : elements) blocks.push_back(make_block(copy(p)));
      }
    });
    return arena->blocks.engine.build(blocks.size(),[&](int i){return blocks[i];});
  }

public:
  using key_type = Key;
  using value_type = S;
  using Entry = std::pair<Key,S>;
  using NodeRef = typename TreeArena<Ops,Backend>::NodeRef;
  SortableSequence(const SortableSequence&) = delete;
  SortableSequence& operator=(const SortableSequence&) = delete;
  SortableSequence(SortableSequence&& other) noexcept
      : arena(std::move(other.arena)), root(std::exchange(other.root,nullptr)){}
  SortableSequence& operator=(SortableSequence&& other) noexcept {
    if(this != &other){arena = std::move(other.arena); root = std::exchange(other.root,nullptr);}
    return *this;
  }
  int size() const {return Blocks::size(root);}
  bool empty() const {return !root;}
  std::size_t allocated_nodes() const {return arena ? arena->entries.allocated_nodes()+arena->blocks.pool.size() : 0;}
  Entry get(int i){auto* p = access(i).second; return {Ops::key(p),p->value};}
  Entry kth(int i){return get(i);}
  [[nodiscard]] NodeRef ref_at(int i){return arena->entries.arena->reference(arena->entries.arena,access(i).second);}
  int index_of(const NodeRef& ref){
    if(!arena || !root) return -1;
    auto& inner = *arena->entries.arena;
    Inner* node = inner.node_of(ref);
    if(!node) return -1;
    Inner* block_root = node;
    while(block_root->pp) block_root = block_root->pp;
    auto* block = static_cast<Node*>(block_root->extra.block);
    int start = arena->blocks.index_of_node(root,block);
    if(start < 0) return -1;
    int offset = inner.index_of_node(block->inner,node);
    attach(block,block->inner);
    return start+(block->descending ? Blocks::local_size(block)-1-offset : offset);
  }
  void insert_at(int i, const Key& key, const S& value){
    assert(0 <= i && i <= size());
    auto [a,b] = split_nodes(root,i);
    auto& engine = arena->blocks.engine;
    root = engine.join(engine.join(a,make_element(key,value)),b);
  }
  void insert_at(int i, const Entry& value){insert_at(i,value.first,value.second);}
  int erase_range(int l, int r){
    return range(l,r,[&](Node*& tree){
      Outer::visit_in_order(tree,[&](Node* p){
        arena->entries.release_tree(p->inner);
        arena->blocks.release(p);
      });
      tree = nullptr;
      return r-l;
    });
  }
  void erase_at(int i){assert(0 <= i && i < size()); erase_range(i,i+1);}
  void set(int i, const Key& key, const S& value){
    assert(0 <= i && i < size());
    range(i,i+1,[&](Node* block){
      Blocks::push(block);
      Inner* p = block->inner;
      Ops::push(p);
      p->extra.key = key;
      p->value = value;
      Ops::pull(p);
      Blocks::pull(block);
      return 0;
    });
  }
  void set(int i, const Entry& value){set(i,value.first,value.second);}
  void set(int i, const S& value){
    auto [block,p] = access(i);
    p->value = value;
    for(;p;p = p->pp) Ops::pull(p);
    for(;block;block = block->pp) Blocks::pull(block);
  }
  void set_at(int i, const S& value){set(i,value);}
  void sort(int l, int r, bool decreasing = false){
    assert(0 <= l && l <= r && r <= size());
    int length = r-l;
    if(length <= 1) return;
    if(Blocks::local_size(root) == size() && root->descending == decreasing) return;
    if(length <= small_sort_limit){
      auto [block,offset] = Links::locate(root,l);
      auto& engine = arena->blocks.engine;
      if constexpr(requires {engine.access_pushed(root,block);}) engine.access_pushed(root,block);
      else engine.access_node(root,block);
      if(length <= Blocks::local_size(block)-offset && block->descending == decreasing) return;
    }
    range(l,r,[&](Node*& tree){
      if(!tree->lp && !tree->rp){
        if(tree->descending != decreasing){
          tree->descending = decreasing;
          std::swap(tree->lsum,tree->rsum);
        }
        return 0;
      }
      auto consume = [&](auto visit){
        Outer::visit_in_order(tree,[&](Node* p){
          visit(p->inner);
          if(p != tree) arena->blocks.release(p);
        });
      };
      Inner* merged = nullptr;
      if(length <= small_sort_limit) merged = sort_small(tree);
      else if(length > bulk_sort_threshold){
        std::vector<Inner*> blocks;
        consume([&](Inner* p){blocks.push_back(p);});
        merged = sort_many(blocks,length);
      }
      else consume([&](Inner* p){merged = meld(merged,p);});
      tree->lp = tree->rp = nullptr;
      tree->descending = decreasing;
      attach(tree,merged);
      Blocks::pull(tree);
      return 0;
    });
  }
  void sort_inc(int l, int r){sort(l,r);}
  void sort_dec(int l, int r){sort(l,r,true);}
  void flip(int l, int r){range(l,r,[](Node* p){Blocks::toggle(p); return 0;});}
  void flip_all(){Blocks::toggle(root);}
  void apply(int l, int r, typename Ops::action_type f) requires Ops::has_action {
    range(l,r,[&](Node* p){Blocks::apply(p,f); return 0;});
  }
  void apply_keys(int l, int r, typename Ops::key_action_type f, bool decreasing = false) requires Ops::has_key_action {
    range(l,r,[&](Node* p){Blocks::apply_keys(p,f,decreasing); return 0;});
  }
  S prod(int l, int r){return fold<false>(l,r);}
  S prodrev(int l, int r){return fold<true>(l,r);}
  S prod_all() const {return root ? root->lsum : Ops::identity();}
  S prodrev_all() const {return root ? root->rsum : Ops::identity();}
  [[nodiscard]] std::pair<SortableSequence,SortableSequence> split(int k){
    assert(arena && 0 <= k && k <= size());
    auto [a,b] = split_nodes(root,k);
    root = nullptr;
    return {SortableSequence(arena,a),SortableSequence(arena,b)};
  }
  void concat(SortableSequence&& other){
    assert(this != &other && arena);
    if(!other.root) return;
    Node* tail = arena == other.arena ? other.root : copy_from(other,true);
    root = arena->blocks.engine.join(root,tail);
    other.root = nullptr;
  }
  [[nodiscard]] SortableSequence clone(){
    SortableSequence copy(arena);
    copy.root = copy.copy_from(*this,false);
    return copy;
  }
  std::vector<Entry> to_vector(){
    std::vector<Entry> result;
    result.reserve(size());
    visit_elements(root,[&](Inner* p){result.emplace_back(Ops::key(p),p->value);});
    return result;
  }
  void clear(){root = nullptr;}
};

template<class Key, class Ops, template<class,class> class Backend, class Balance, class Compare>
class SortableArena {
public:
  using Sequence = SortableSequence<Key,Ops,Backend,Balance,Compare>;
  using Entry = typename Sequence::Entry;
  using NodeRef = typename Sequence::NodeRef;
private:
  using Storage = typename Sequence::Storage;
  std::shared_ptr<Storage> arena;
public:
  explicit SortableArena(Compare compare = Compare()) : arena(std::make_shared<Storage>(std::move(compare))){}
  SortableArena(const SortableArena&) = delete;
  SortableArena& operator=(const SortableArena&) = delete;
  SortableArena(SortableArena&&) noexcept = default;
  SortableArena& operator=(SortableArena&&) noexcept = default;
  [[nodiscard]] Sequence new_sequence(){assert(arena); return Sequence(arena);}
  [[nodiscard]] Sequence new_sequence(const std::vector<Entry>& values){
    auto result = new_sequence();
    result.root = arena->blocks.engine.build(values.size(),[&](int i){
      return result.make_element(values[i].first,values[i].second);
    });
    return result;
  }
  std::size_t allocated_nodes() const{
    return arena ? arena->entries.allocated_nodes()+arena->blocks.pool.size() : 0;
  }
};

}  // namespace soryuusi::internal
