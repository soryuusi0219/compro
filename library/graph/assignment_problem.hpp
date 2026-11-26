// https://github.com/soryuusi0219/compro/blob/main/library/graph/assignment_problem.hpp
#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

namespace soryuusi {

struct AssignmentResult {
  __int128_t cost = 0;
  std::vector<int> left_to_right, right_to_left;
  std::vector<__int128_t> left_potential, right_potential;
};

namespace internal {

template <class Price, class Row> bool monge_assignment(int n, const Row& row, std::vector<int>& left, std::vector<int>& right, std::vector<Price>& u, std::vector<Price>& v) {
  int order = 3;
  for(int i = 1; i < n && order; ++i) {
    const auto& top = row(i-1);
    const auto& bottom = row(i);
    for(int j = 1; j < n; ++j) {
      const Price diagonal = Price(top[j-1])+bottom[j], crossing = Price(top[j])+bottom[j-1];
      if(diagonal > crossing) order &= 2;
      if(diagonal < crossing) order &= 1;
      if(!order) break;
    }
  }
  if(!order) return false;
  const int step = order&1 ? 1 : -1;
  int col = step > 0 ? 0 : n-1;
  v[col] = 0;
  for(int i = 0; i < n; ++i, col += step) {
    left[i] = col;
    right[col] = i;
    // Adjacent column differences telescope to feasible prices by Monge.
    if(i+1 < n) v[col+step] = v[col]+Price(row(i)[col+step])-row(i)[col];
  }
  const Price shift = *std::max_element(v.begin(),v.end());
  for(Price& p : v) p -= shift;
  for(int i = 0; i < n; ++i) u[i] = Price(row(i)[left[i]])-v[left[i]];
  return true;
}

template <class Price, class Row> void assignment_column_reduction(int n, const Row& row, std::vector<int>& left, std::vector<int>& right, std::vector<Price>& u, std::vector<Price>& v) {
  constexpr Price inf = std::numeric_limits<Price>::max();
  std::fill(left.begin(),left.end(),-1);
  std::fill(right.begin(),right.end(),-1);
  std::fill(v.begin(),v.end(),inf);
  std::vector<int> owner(n), count(n,0), free;
  free.reserve(n);
  for(int i = 0; i < n; ++i) {
    const auto& costs = row(i);
    for(int j = 0; j < n; ++j) if(Price(costs[j]) < v[j]) {
      v[j] = costs[j];
      owner[j] = i;
    }
  }
  for(int j = n-1; j >= 0; --j) {
    const int i = owner[j];
    if(count[i]++ == 0) left[i] = j, right[j] = i;
  }
  for(int i = 0; i < n; ++i) if(count[i] == 1) {
    const auto& costs = row(i);
    const int matched = left[i];
    Price alternative = inf;
    for(int j = 0; j < n; ++j) if(j != matched) alternative = std::min(alternative,Price(costs[j])-v[j]);
    v[matched] -= alternative;
  }

  // A bounded number of row-reduction passes shrinks the free set before Dijkstra.
  for(int pass = 0; pass < 2; ++pass) {
    free.clear();
    for(int i = 0; i < n; ++i) if(left[i] < 0) free.push_back(i);
    for(int i : free) {
      const auto& costs = row(i);
      Price best = inf, second = inf;
      int chosen = -1;
      for(int j = 0; j < n; ++j) {
        const Price x = Price(costs[j])-v[j];
        if(x < best) {
          second = best;
          best = x;
          chosen = j;
        } else if(x <= second) {
          second = x;
          if(x == best && right[j] < 0) chosen = j;
        }
      }
      v[chosen] -= second-best;
      if(right[chosen] >= 0) left[right[chosen]] = -1;
      left[i] = chosen;
      right[chosen] = i;
    }
  }
  const Price shift = *std::max_element(v.begin(),v.end());
  for(Price& price : v) price -= shift;
  for(int i = 0; i < n; ++i) {
    const auto& costs = row(i);
    if(left[i] >= 0) u[i] = Price(costs[left[i]])-v[left[i]];
    else {
      u[i] = inf;
      for(int j = 0; j < n; ++j) u[i] = std::min(u[i],Price(costs[j])-v[j]);
    }
  }
}

template <bool Complete, class Price, class Row, class Allowed> std::optional<AssignmentResult> hungarian(int n, int m, const Row& row, const Allowed& allowed) {
  if(n > m) return std::nullopt;
  constexpr Price inf = std::numeric_limits<Price>::max();
  AssignmentResult result;
  auto& left = result.left_to_right;
  auto& right = result.right_to_left;
  left.assign(n,-1);
  right.assign(m,-1);
  std::vector<Price> u(n), v(m,0);
  for(int i = 0; i < n; ++i) {
    const auto& cost = row(i);
    Price best = inf;
    int chosen = -1;
    for(int j = 0; j < m; ++j) if(allowed(i,j)) {
      const Price c = cost[j];
      if(c < best) {
        best = c;
        chosen = -1;
      }
      if(c == best && chosen < 0 && right[j] < 0) chosen = j;
    }
    if(best == inf) return std::nullopt;
    u[i] = best;
    if(chosen >= 0) {
      left[i] = chosen;
      right[chosen] = i;
    }
  }

  bool finished = std::find(left.begin(),left.end(),-1) == left.end();
  if constexpr(Complete) {
    if(!finished && n == m) {
      finished = monge_assignment(n,row,left,right,u,v);
      if(!finished && n >= 32) assignment_column_reduction(n,row,left,right,u,v);
    }
  }
  if(!finished) {
    std::vector<Price> distance(m);
    std::vector<int> parent(m), pending(m), tree;
    tree.reserve(n);
    for(int root = 0; root < n; ++root) if(left[root] < 0) {
      std::fill(distance.begin(),distance.end(),inf);
      std::iota(pending.begin(),pending.end(),0);
      tree.clear();
      int remaining = m, i = root, previous = -1;
      Price d = 0;
      while(true) {
        const auto& cost = row(i);
        const Price offset = d-u[i];
        Price best = inf;
        int chosen = -1;
        for(int k = 0; k < remaining; ++k) {
          const int j = pending[k];
          if(allowed(i,j)) {
            const Price candidate = Price(cost[j])+offset-v[j];
            if(candidate < distance[j]) {
              distance[j] = candidate;
              parent[j] = previous;
            }
          }
          if(distance[j] < best || (distance[j] == best && best != inf && right[j] < 0)) {
            best = distance[j];
            chosen = k;
          }
        }
        if(chosen < 0) return std::nullopt;
        const int j = pending[chosen];
        if(right[j] >= 0) {
          pending[chosen] = pending[--remaining];
          tree.push_back(j);
          i = right[j];
          previous = j;
          d = best;
          continue;
        }

        // Only the settled tree needs a dual adjustment; its distances are fixed.
        u[root] += best;
        for(int k : tree) {
          const Price delta = best-distance[k];
          u[right[k]] += delta;
          v[k] -= delta;
        }
        for(int col = j; col >= 0; ) {
          const int before = parent[col];
          const int r = before < 0 ? root : right[before];
          left[r] = col;
          right[col] = r;
          col = before;
        }
        break;
      }
    }
  }
  for(int i = 0; i < n; ++i) result.cost += __int128_t(row(i)[left[i]]);
  result.left_potential.assign(u.begin(),u.end());
  result.right_potential.assign(v.begin(),v.end());
  return result;
}

template <class Cost, bool Complete, class Row, class Allowed> std::optional<AssignmentResult> assignment_dispatch(int n, int m, const Row& row, const Allowed& allowed) {
  static_assert(std::numeric_limits<Cost>::is_integer && std::numeric_limits<Cost>::is_signed && sizeof(Cost) <= 8);
  if(n > m) return std::nullopt;
  const auto limit = std::numeric_limits<std::int64_t>::max()/(32*(__int128_t(n)+1));
  if(__int128_t(std::numeric_limits<Cost>::max())+1 <= limit) return hungarian<Complete,std::int64_t>(n,m,row,allowed);
  for(int i = 0; i < n; ++i) for(Cost x : row(i)) {
    if(__int128_t(x) < -limit || __int128_t(x) > limit) return hungarian<Complete,__int128_t>(n,m,row,allowed);
  }
  return hungarian<Complete,std::int64_t>(n,m,row,allowed);
}

} // namespace internal

template <class Cost> std::optional<AssignmentResult> assignment_problem(int n, int m, std::span<const Cost> costs, std::span<const unsigned char> allowed = {}) {
  if(n < 0 || m < 0 || (m != 0 && std::size_t(n) > costs.size()/std::size_t(m)) || costs.size() != std::size_t(n)*m) {
    throw std::invalid_argument("invalid assignment matrix shape");
  }
  if(!allowed.empty() && allowed.size() != costs.size()) throw std::invalid_argument("invalid assignment mask shape");
  const auto row = [&](int i) { return costs.subspan(std::size_t(i)*m,m); };
  if(allowed.empty()) return internal::assignment_dispatch<Cost,true>(n,m,row,[](int, int) { return true; });
  return internal::assignment_dispatch<Cost,false>(n,m,row,[&](int i, int j) { return allowed[std::size_t(i)*m+j] != 0; });
}

template <class Cost> std::optional<AssignmentResult> assignment_problem(const std::vector<std::vector<Cost>>& costs, const std::vector<std::vector<bool>>& allowed = {}) {
  const std::size_t m = costs.empty() ? 0 : costs.front().size();
  if(costs.size() > std::numeric_limits<int>::max() || m > std::numeric_limits<int>::max()) throw std::length_error("assignment matrix too large");
  for(const auto& row : costs) if(row.size() != m) throw std::invalid_argument("invalid assignment matrix shape");
  if(!allowed.empty()) {
    if(allowed.size() != costs.size()) throw std::invalid_argument("invalid assignment mask shape");
    for(const auto& row : allowed) if(row.size() != m) throw std::invalid_argument("invalid assignment mask shape");
  }
  const auto row = [&](int i) -> const std::vector<Cost>& { return costs[i]; };
  if(allowed.empty()) return internal::assignment_dispatch<Cost,true>(costs.size(),m,row,[](int, int) { return true; });
  return internal::assignment_dispatch<Cost,false>(costs.size(),m,row,[&](int i, int j) { return allowed[i][j]; });
}

} // namespace soryuusi
