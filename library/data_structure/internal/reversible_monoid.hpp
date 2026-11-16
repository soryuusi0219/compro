// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/internal/reversible_monoid.hpp
#pragma once
#include <type_traits>
#include <utility>

namespace soryuusi::internal {

struct NoTreeAction {
  struct value_type {};
  struct State {};
  static constexpr bool enabled = false;
};
template<class F, auto mapping, auto composition, auto id>
struct TreeAction {
  static_assert(std::is_invocable_r_v<F,decltype(composition),F,F>);
  static_assert(std::is_invocable_r_v<F,decltype(id)>);
  using value_type = F;
  struct State {F lazy = id();};
  static constexpr bool enabled = true;
  static F identity(){return id();}
  static F compose(F f, F g){return composition(f,g);}
  template<class T> static T map(F f, T x){return mapping(f,x);}
};
struct EmptyTreeState {};
template<bool Enabled> struct TreeCount {};
template<> struct TreeCount<true> {int size = 1;};
template<bool Enabled> struct TreeIndex {};
template<> struct TreeIndex<true> {int index = -1;};

// Node layout and augmentation are independent of balancing and allocation.
template<class T, auto op, auto e, class Action = NoTreeAction,
         bool Counted = true, bool Indexed = false, class Extra = EmptyTreeState>
struct ReversibleMonoid {
  static_assert(std::is_invocable_r_v<T,decltype(op),T,T>);
  static_assert(std::is_invocable_r_v<T,decltype(e)>);
  using value_type = T;
  using action_type = typename Action::value_type;
  static constexpr bool has_action = Action::enabled;
  struct Node {
    Node* pp = nullptr;
    Node* lp = nullptr;
    Node* rp = nullptr;
    T value, lsum, rsum;
    bool rev = false;
    [[no_unique_address]] TreeCount<Counted> count;
    [[no_unique_address]] TreeIndex<Indexed> label;
    [[no_unique_address]] typename Action::State action;
    [[no_unique_address]] Extra extra;
    explicit Node(const T& x = e()) : value(x), lsum(x), rsum(x){}
    Node(const T& x, Extra state) : value(x), lsum(x), rsum(x), extra(std::move(state)){}
  };
  static T identity(){return e();}
  static int size(const Node* p) requires Counted {return p ? p->count.size : 0;}
  static int local_size(const Node*) requires Counted {return 1;}
  static auto& priority(Node* p) requires requires {p->extra.priority;} {return p->extra.priority;}
  static void pull(Node* p){
    p->lsum = p->rsum = p->value;
    if constexpr(Counted) p->count.size = 1+size(p->lp)+size(p->rp);
    if(p->lp){p->lsum = op(p->lp->lsum,p->lsum); p->rsum = op(p->rsum,p->lp->rsum);}
    if(p->rp){p->lsum = op(p->lsum,p->rp->lsum); p->rsum = op(p->rp->rsum,p->rsum);}
  }
  static void toggle(Node* p){
    if(!p) return;
    std::swap(p->lp,p->rp); std::swap(p->lsum,p->rsum); p->rev ^= true;
  }
  static void apply(Node* p, const action_type& f) requires has_action {
    if(!p) return;
    p->value = Action::map(f,p->value);
    p->lsum = Action::map(f,p->lsum);
    p->rsum = Action::map(f,p->rsum);
    p->action.lazy = Action::compose(f,p->action.lazy);
  }
  static void push(Node* p){
    if constexpr(has_action){
      if(p->action.lazy != Action::identity()){
        apply(p->lp,p->action.lazy); apply(p->rp,p->action.lazy);
        p->action.lazy = Action::identity();
      }
    }
    if(p->rev){toggle(p->lp); toggle(p->rp); p->rev = false;}
  }
};

}  // namespace soryuusi::internal
