// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/segtree_template.hpp
#pragma once
#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/segtree.hpp"

namespace soryuusi {

namespace internal {

  template<class T>
  struct segtree_sum {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return x+y;};
    static constexpr T (*e)() = []()->T {return T(0);};
  };

  template<class T, T INF>
  struct segtree_min {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return std::min(x,y);};
    static constexpr T (*e)() = []()->T {return INF;};
  };

  template<class T, class U, T INF>
  struct segtree_min_count {
    using S = std::pair<T,U>;
    static constexpr S (*op)(S,S) = [](S x, S y)->S {
      if(x.first == y.first) return {x.first,x.second+y.second};
      if(x.first < y.first) return x;
      return y;
    };
    static constexpr S (*e)() = []()->S {return {INF,U(0)};};
    using Base = Segtree<S,op,e>;
  };

  template<class T, T LOWEST>
  struct segtree_max {
    static constexpr T (*op)(T,T) = [](T x, T y)->T {return std::max(x,y);};
    static constexpr T (*e)() = []()->T {return LOWEST;};
  };

  template<class T, class U, T LOWEST>
  struct segtree_max_count {
    using S = std::pair<T,U>;
    static constexpr S (*op)(S,S) = [](S x, S y)->S {
      if(x.first == y.first) return {x.first,x.second+y.second};
      if(x.first > y.first) return x;
      return y;
    };
    static constexpr S (*e)() = []()->S {return {LOWEST,U(0)};};
    using Base = Segtree<S,op,e>;
  };

  template<class T, class U>
  std::vector<std::pair<T,U>> segtree_count_values(const std::vector<T>& v) {
    std::vector<std::pair<T,U>> data;
    data.reserve(v.size());
    for(const auto& x : v) data.emplace_back(x,U(1));
    return data;
  }

  template<class T, class U>
  struct segtree_pair_sum {
    using S = std::pair<T,U>;
    static constexpr S (*op)(S,S) = [](S x, S y)->S {return {x.first+y.first,x.second+y.second};};
    static constexpr S (*e)() = []()->S {return {T(0),U(0)};};
  };

  template<class T>
  struct segtree_composite {
    using S = std::pair<T,T>;
    static constexpr S (*op)(S,S) = [](S x, S y)->S {return {y.first*x.first,y.first*x.second+y.second};};
    static constexpr S (*e)() = []()->S {return {T(1),T(0)};};
  };

}  // namespace internal

  // Addition (integers, modint, etc.)
  template<class T>
  using SegtreeSum = Segtree<T,internal::segtree_sum<T>::op,internal::segtree_sum<T>::e>;

  // Minimum
  template<class T, T INF = std::numeric_limits<T>::max()/2>
  using SegtreeMin = Segtree<T,internal::segtree_min<T,INF>::op,internal::segtree_min<T,INF>::e>;

  // Minimum + count
  template<class T, class U = int, T INF = std::numeric_limits<T>::max()/2>
  struct SegtreeMinCount : internal::segtree_min_count<T,U,INF>::Base {
    using Operations = internal::segtree_min_count<T,U,INF>;
    using Base = typename Operations::Base;
    using value_type = typename Operations::S;
    using Base::Base;

    SegtreeMinCount() = default;
    template<class V> requires std::is_same_v<T,V>
    SegtreeMinCount(const std::vector<V>& v) : Base(internal::segtree_count_values<T,U>(v)) {}
  };

  // Maximum
  template<class T, T LOWEST = std::numeric_limits<T>::lowest()/2>
  using SegtreeMax = Segtree<T,internal::segtree_max<T,LOWEST>::op,internal::segtree_max<T,LOWEST>::e>;

  // Maximum + count
  template<class T, class U = int, T LOWEST = std::numeric_limits<T>::lowest()/2>
  struct SegtreeMaxCount : internal::segtree_max_count<T,U,LOWEST>::Base {
    using Operations = internal::segtree_max_count<T,U,LOWEST>;
    using Base = typename Operations::Base;
    using value_type = typename Operations::S;
    using Base::Base;

    SegtreeMaxCount() = default;
    template<class V> requires std::is_same_v<T,V>
    SegtreeMaxCount(const std::vector<V>& v) : Base(internal::segtree_count_values<T,U>(v)) {}
  };

  // Component-wise pair addition
  template<class T, class U = T>
  using SegtreePairSum = Segtree<std::pair<T,U>,internal::segtree_pair_sum<T,U>::op,internal::segtree_pair_sum<T,U>::e>;

  // Affine composition: op(f,g) = g(f(x)), f(x) = f.first*x + f.second
  template<class T>
  using SegtreeComposite = Segtree<std::pair<T,T>,internal::segtree_composite<T>::op,internal::segtree_composite<T>::e>;

}  // namespace soryuusi
