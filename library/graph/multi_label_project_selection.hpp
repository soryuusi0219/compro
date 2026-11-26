// https://github.com/soryuusi0219/compro/blob/main/library/graph/multi_label_project_selection.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>
#include "internal/selection_cut.hpp"

namespace soryuusi {

template <class Cost = long long> class MultiLabelProjectSelection {
  using Cut = internal::SelectionCut<Cost>;
  std::vector<int> offset;
  Cut cut;

  static std::vector<int> thresholds(const std::vector<int>& count) {
    if(count.size() > std::numeric_limits<int>::max()) throw std::length_error("too many selection variables");
    std::vector<int> offset;
    offset.reserve(count.size()+1);
    offset.push_back(2);
    for(int k : count) {
      if(k <= 0) throw std::invalid_argument("at least one label is required");
      if(k-1 > std::numeric_limits<int>::max()-offset.back()) throw std::length_error("too many threshold vertices");
      offset.push_back(offset.back()+(k-1));
    }
    return offset;
  }

  int labels(int i) const {
    assert(0 <= i && std::size_t(i)+1 < offset.size());
    return offset[i+1]-offset[i]+1;
  }

  int threshold(int i, int a) const { return offset[i]+(a-1); }

public:
  using Value = __int128_t;

  struct Result {
    Value cost;
    std::vector<int> label;
  };

  explicit MultiLabelProjectSelection(const std::vector<int>& count) : offset(thresholds(count)), cut(offset.back()-2) {
    for(std::size_t i = 0; i < count.size(); ++i) {
      for(int v = offset[i+1]; v-offset[i] > 1; --v) cut.imply(v-1,v-2);
    }
  }

  void reserve_edges(int m) { cut.reserve_edges(m); }
  void add_constant(Cost cost) { cut.constant += cost; }

  void add_unary_cost(int i, std::span<const Cost> costs) {
    const int k = labels(i);
    if(costs.size() != std::size_t(k)) throw std::invalid_argument("incorrect label cost count");
    cut.constant += costs[0];
    for(int a = 1; a < k; ++a) cut.add_delta(threshold(i,a),Value(costs[a])-costs[a-1]);
  }

  void add_pair_cost(int i, int j, const std::vector<std::vector<Cost>>& cost) {
    const int h = labels(i), w = labels(j);
    if(i == j || cost.size() != std::size_t(h)) throw std::invalid_argument("incorrect label pair");
    for(const auto& row : cost) if(row.size() != std::size_t(w)) throw std::invalid_argument("incorrect label cost count");
    const auto difference = [&](int a, int b) { return Value(cost[a-1][b])+cost[a][b-1]-cost[a-1][b-1]-cost[a][b]; };
    for(int a = 1; a < h; ++a) for(int b = 1; b < w; ++b) {
      if(difference(a,b) < 0) throw std::invalid_argument("pair cost must be Monge");
    }
    for(int a = 1; a < h; ++a) for(int b = 1; b < w; ++b) {
      cut.add_arc(threshold(i,a),threshold(j,b),difference(a,b));
    }
    cut.constant += cost[0][0];
    for(int a = 1; a < h; ++a) cut.add_delta(threshold(i,a),Value(cost[a][w-1])-cost[a-1][w-1]);
    for(int b = 1; b < w; ++b) cut.add_delta(threshold(j,b),Value(cost[0][b])-cost[0][b-1]);
  }

  void fix(int i, int label) {
    const int k = labels(i);
    assert(0 <= label && label < k);
    if(label > 0) cut.imply(Cut::source,threshold(i,label));
    if(label+1 < k) cut.imply(threshold(i,label+1),Cut::sink);
  }

  std::optional<Result> solve() const { return solve_impl(true); }
  std::optional<Result> solve_dinic() const { return solve_impl(false); }

private:
  std::optional<Result> solve_impl(bool push_relabel) const {
    auto answer = cut.solve(push_relabel);
    if(!answer) return std::nullopt;
    auto& [cost,side] = *answer;
    std::vector<int> assignment(offset.size()-1);
    for(std::size_t i = 0; i < assignment.size(); ++i) {
      for(int v = offset[i]; v < offset[i+1] && side[v]; ++v) ++assignment[i];
    }
    return Result{cost,std::move(assignment)};
  }
};

} // namespace soryuusi
