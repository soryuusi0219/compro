// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_beats_templates.hpp
#pragma once
#include <algorithm>
#include <limits>
#include <optional>
#include <type_traits>
#include <vector>

#include "library/data_structure/segtree_beats.hpp"

namespace soryuusi {

namespace internal {

  template<class T, class Sum>
  struct segtree_beats_chmin_chmax_add {
    struct S {
      Sum sum = 0;
      int len = 0;
      T mx1 = std::numeric_limits<T>::lowest(), mx2 = std::numeric_limits<T>::lowest();
      int mxc = 0;
      T mn1 = std::numeric_limits<T>::max(), mn2 = std::numeric_limits<T>::max();
      int mnc = 0;
      bool fail = false;
      S() = default;
      S(T x, int count = 1) : sum(Sum(x)*count), len(count), mx1(x), mxc(count), mn1(x), mnc(count) {}
    };
    // x -> clamp(x + delta, lower, upper); missing bounds are unbounded.
    struct F {
      T delta = 0;
      std::optional<T> lower, upper;
      static F chmin(T x) { F f; f.upper = x; return f; }
      static F chmax(T x) { F f; f.lower = x; return f; }
      static F add(T x) { F f; f.delta = x; return f; }
    };

    static constexpr S (*op)(S,S) = [](S x, S y)->S {
      if(!x.len) return y;
      if(!y.len) return x;
      S ret;
      ret.sum = x.sum+y.sum;
      ret.len = x.len+y.len;
      if(x.mx1 == y.mx1) {
        ret.mx1 = x.mx1;
        ret.mx2 = std::max(x.mx2,y.mx2);
        ret.mxc = x.mxc+y.mxc;
      } else if(x.mx1 > y.mx1) {
        ret.mx1 = x.mx1;
        ret.mx2 = std::max(x.mx2,y.mx1);
        ret.mxc = x.mxc;
      } else {
        ret.mx1 = y.mx1;
        ret.mx2 = std::max(x.mx1,y.mx2);
        ret.mxc = y.mxc;
      }
      if(x.mn1 == y.mn1) {
        ret.mn1 = x.mn1;
        ret.mn2 = std::min(x.mn2,y.mn2);
        ret.mnc = x.mnc+y.mnc;
      } else if(x.mn1 < y.mn1) {
        ret.mn1 = x.mn1;
        ret.mn2 = std::min(x.mn2,y.mn1);
        ret.mnc = x.mnc;
      } else {
        ret.mn1 = y.mn1;
        ret.mn2 = std::min(x.mn1,y.mn2);
        ret.mnc = y.mnc;
      }
      return ret;
    };
    static constexpr S (*e)() = []()->S {return S();};
    static constexpr S (*mapping)(F,S) = [](F f, S x)->S {
      if(!x.len) return x;
      x.sum += Sum(f.delta)*x.len;
      x.mx1 += f.delta;
      x.mn1 += f.delta;
      if(x.mxc != x.len) x.mx2 += f.delta;
      if(x.mnc != x.len) x.mn2 += f.delta;
      if(f.lower && x.mn1 < *f.lower) {
        T bound = *f.lower;
        if(x.mx1 <= bound) return S(bound,x.len);
        if(x.mn2 <= bound) { x.fail = true; return x; }
        x.sum += (Sum(bound)-Sum(x.mn1))*x.mnc;
        if(x.mx2 == x.mn1) x.mx2 = bound;
        x.mn1 = bound;
      }
      if(f.upper && *f.upper < x.mx1) {
        T bound = *f.upper;
        if(bound <= x.mn1) return S(bound,x.len);
        if(bound <= x.mx2) { x.fail = true; return x; }
        x.sum += (Sum(bound)-Sum(x.mx1))*x.mxc;
        if(x.mn2 == x.mx1) x.mn2 = bound;
        x.mx1 = bound;
      }
      return x;
    };
    // composition(f,g) applies g first, then f.
    static constexpr F (*composition)(F,F) = [](F f, F g)->F {
      auto transform = [&](T x)->T {
        x += f.delta;
        if(f.lower) x = std::max(x,*f.lower);
        if(f.upper) x = std::min(x,*f.upper);
        return x;
      };
      g.delta += f.delta;
      g.lower = g.lower ? std::optional<T>(transform(*g.lower)) : f.lower;
      g.upper = g.upper ? std::optional<T>(transform(*g.upper)) : f.upper;
      return g;
    };
    static constexpr F (*id)() = []()->F {return F();};
    using Base = segtree_beats<S,op,e,F,mapping,composition,id>;
  };

  template<class T, class Sum>
  struct segtree_beats_chmin_add {
    struct S {
      Sum sum = 0;
      int len = 0;
      T mx1 = std::numeric_limits<T>::lowest(), mx2 = std::numeric_limits<T>::lowest();
      int mxc = 0;
      T mn1 = std::numeric_limits<T>::max();
      bool fail = false;
      S() = default;
      S(T x, int count = 1) : sum(Sum(x)*count), len(count), mx1(x), mxc(count), mn1(x) {}
    };
    struct F {
      T delta = 0;
      std::optional<T> upper;
      static F chmin(T x) { F f; f.upper = x; return f; }
      static F add(T x) { F f; f.delta = x; return f; }
    };

    static constexpr S (*op)(S,S) = [](S x, S y)->S {
      if(!x.len) return y;
      if(!y.len) return x;
      S ret;
      ret.sum = x.sum+y.sum;
      ret.len = x.len+y.len;
      ret.mn1 = std::min(x.mn1,y.mn1);
      if(x.mx1 == y.mx1) {
        ret.mx1 = x.mx1;
        ret.mx2 = std::max(x.mx2,y.mx2);
        ret.mxc = x.mxc+y.mxc;
      } else if(x.mx1 > y.mx1) {
        ret.mx1 = x.mx1;
        ret.mx2 = std::max(x.mx2,y.mx1);
        ret.mxc = x.mxc;
      } else {
        ret.mx1 = y.mx1;
        ret.mx2 = std::max(x.mx1,y.mx2);
        ret.mxc = y.mxc;
      }
      return ret;
    };
    static constexpr S (*e)() = []()->S {return S();};
    static constexpr S (*mapping)(F,S) = [](F f, S x)->S {
      if(!x.len) return x;
      x.sum += Sum(f.delta)*x.len;
      x.mx1 += f.delta;
      x.mn1 += f.delta;
      if(x.mxc != x.len) x.mx2 += f.delta;
      if(f.upper && *f.upper < x.mx1) {
        T bound = *f.upper;
        if(bound <= x.mn1) return S(bound,x.len);
        if(bound <= x.mx2) { x.fail = true; return x; }
        x.sum += (Sum(bound)-Sum(x.mx1))*x.mxc;
        x.mx1 = bound;
      }
      return x;
    };
    static constexpr F (*composition)(F,F) = [](F f, F g)->F {
      g.delta += f.delta;
      if(g.upper) *g.upper += f.delta;
      if(f.upper) g.upper = g.upper ? std::min(*g.upper,*f.upper) : *f.upper;
      return g;
    };
    static constexpr F (*id)() = []()->F {return F();};
    using Base = segtree_beats<S,op,e,F,mapping,composition,id>;
  };

  template<class T, class Sum>
  struct segtree_beats_chmax_add {
    struct S {
      Sum sum = 0;
      int len = 0;
      T mx1 = std::numeric_limits<T>::lowest();
      T mn1 = std::numeric_limits<T>::max(), mn2 = std::numeric_limits<T>::max();
      int mnc = 0;
      bool fail = false;
      S() = default;
      S(T x, int count = 1) : sum(Sum(x)*count), len(count), mx1(x), mn1(x), mnc(count) {}
    };
    struct F {
      T delta = 0;
      std::optional<T> lower;
      static F chmax(T x) { F f; f.lower = x; return f; }
      static F add(T x) { F f; f.delta = x; return f; }
    };

    static constexpr S (*op)(S,S) = [](S x, S y)->S {
      if(!x.len) return y;
      if(!y.len) return x;
      S ret;
      ret.sum = x.sum+y.sum;
      ret.len = x.len+y.len;
      ret.mx1 = std::max(x.mx1,y.mx1);
      if(x.mn1 == y.mn1) {
        ret.mn1 = x.mn1;
        ret.mn2 = std::min(x.mn2,y.mn2);
        ret.mnc = x.mnc+y.mnc;
      } else if(x.mn1 < y.mn1) {
        ret.mn1 = x.mn1;
        ret.mn2 = std::min(x.mn2,y.mn1);
        ret.mnc = x.mnc;
      } else {
        ret.mn1 = y.mn1;
        ret.mn2 = std::min(x.mn1,y.mn2);
        ret.mnc = y.mnc;
      }
      return ret;
    };
    static constexpr S (*e)() = []()->S {return S();};
    static constexpr S (*mapping)(F,S) = [](F f, S x)->S {
      if(!x.len) return x;
      x.sum += Sum(f.delta)*x.len;
      x.mx1 += f.delta;
      x.mn1 += f.delta;
      if(x.mnc != x.len) x.mn2 += f.delta;
      if(f.lower && x.mn1 < *f.lower) {
        T bound = *f.lower;
        if(x.mx1 <= bound) return S(bound,x.len);
        if(x.mn2 <= bound) { x.fail = true; return x; }
        x.sum += (Sum(bound)-Sum(x.mn1))*x.mnc;
        x.mn1 = bound;
      }
      return x;
    };
    static constexpr F (*composition)(F,F) = [](F f, F g)->F {
      g.delta += f.delta;
      if(g.lower) *g.lower += f.delta;
      if(f.lower) g.lower = g.lower ? std::max(*g.lower,*f.lower) : *f.lower;
      return g;
    };
    static constexpr F (*id)() = []()->F {return F();};
    using Base = segtree_beats<S,op,e,F,mapping,composition,id>;
  };

}  // namespace internal

// Query sum, minimum (mn1), and maximum (mx1). Sum can be wider than T.
template<class T, class Sum = T>
struct SegtreeBeatsChminChmaxAdd : internal::segtree_beats_chmin_chmax_add<T,Sum>::Base {
  using Operations = internal::segtree_beats_chmin_chmax_add<T,Sum>;
  using Base = typename Operations::Base;
  using value_type = typename Operations::S;
  using action_type = typename Operations::F;
  using Base::Base;

  SegtreeBeatsChminChmaxAdd() = default;
  template<class U> requires std::is_same_v<T,U>
  SegtreeBeatsChminChmaxAdd(const std::vector<U>& v) : Base(std::vector<value_type>(v.begin(),v.end())) {}

  void chmin(int l, int r, T x) { this->apply(l, r, action_type::chmin(x)); }
  void chmax(int l, int r, T x) { this->apply(l, r, action_type::chmax(x)); }
  void add(int l, int r, T x) { this->apply(l, r, action_type::add(x)); }
};

template<class T, class Sum = T>
struct SegtreeBeatsChminAdd : internal::segtree_beats_chmin_add<T,Sum>::Base {
  using Operations = internal::segtree_beats_chmin_add<T,Sum>;
  using Base = typename Operations::Base;
  using value_type = typename Operations::S;
  using action_type = typename Operations::F;
  using Base::Base;

  SegtreeBeatsChminAdd() = default;
  template<class U> requires std::is_same_v<T,U>
  SegtreeBeatsChminAdd(const std::vector<U>& v) : Base(std::vector<value_type>(v.begin(),v.end())) {}

  void chmin(int l, int r, T x) { this->apply(l, r, action_type::chmin(x)); }
  void add(int l, int r, T x) { this->apply(l, r, action_type::add(x)); }
};

template<class T, class Sum = T>
struct SegtreeBeatsChmaxAdd : internal::segtree_beats_chmax_add<T,Sum>::Base {
  using Operations = internal::segtree_beats_chmax_add<T,Sum>;
  using Base = typename Operations::Base;
  using value_type = typename Operations::S;
  using action_type = typename Operations::F;
  using Base::Base;

  SegtreeBeatsChmaxAdd() = default;
  template<class U> requires std::is_same_v<T,U>
  SegtreeBeatsChmaxAdd(const std::vector<U>& v) : Base(std::vector<value_type>(v.begin(),v.end())) {}

  void chmax(int l, int r, T x) { this->apply(l, r, action_type::chmax(x)); }
  void add(int l, int r, T x) { this->apply(l, r, action_type::add(x)); }
};

}  // namespace soryuusi
