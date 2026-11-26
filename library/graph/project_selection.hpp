// https://github.com/soryuusi0219/compro/blob/main/library/graph/project_selection.hpp
#pragma once
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>
#include "internal/selection_cut.hpp"

namespace soryuusi {

template <class Cost = long long> class ProjectSelection {
  using Cut = internal::SelectionCut<Cost>;
  int n;
  Cut cut;

  static int checked_size(int n) {
    if(n < 0 || n > std::numeric_limits<int>::max()-2) throw std::length_error("invalid selection size");
    return n;
  }

  int vertex(int i) const {
    assert(0 <= i && i < n);
    return i+2;
  }

  void group_profit(std::span<const int> variables, Cost profit, bool value) {
    if(profit < 0) throw std::invalid_argument("group profit must be nonnegative");
    for(int i : variables) vertex(i);
    if(profit == 0) return;
    if(variables.empty()) {
      cut.constant -= profit;
    } else if(variables.size() == 1) {
      add_profit(variables.front(),profit,value);
    } else {
      const int auxiliary = cut.add_vertex();
      if(value) {
        cut.add_delta(auxiliary,-Value(profit));
        for(int i : variables) cut.imply(auxiliary,vertex(i));
      } else {
        cut.constant -= profit;
        cut.add_delta(auxiliary,profit);
        for(int i : variables) cut.imply(vertex(i),auxiliary);
      }
    }
  }

public:
  using Value = __int128_t;

  struct Result {
    Value cost;
    std::vector<bool> selected;
  };

  explicit ProjectSelection(int n = 0) : n(checked_size(n)), cut(n) {}

  void reserve_edges(int m) { cut.reserve_edges(m); }
  void add_constant(Cost cost) { cut.constant += cost; }

  void add_unary_cost(int i, Cost false_cost, Cost true_cost) {
    cut.add_delta(vertex(i),Value(true_cost)-false_cost);
    cut.constant += false_cost;
  }

  void add_cost(int i, Cost cost, bool value = true) {
    cut.add_delta(vertex(i),value ? Value(cost) : -Value(cost));
    if(!value) cut.constant += cost;
  }

  void add_profit(int i, Cost profit, bool value = true) {
    cut.add_delta(vertex(i),value ? -Value(profit) : Value(profit));
    if(!value) cut.constant -= profit;
  }

  void add_pair_cost(int i, int j, Cost f00, Cost f01, Cost f10, Cost f11) {
    const int u = vertex(i), v = vertex(j);
    const Value cap = Value(f01)+f10-f00-f11;
    if(cap < 0) throw std::invalid_argument("pair cost must be submodular");
    if(u == v) {
      add_unary_cost(i,f00,f11);
      return;
    }
    cut.add_arc(u,v,cap);
    cut.add_delta(u,Value(f11)-f01);
    cut.add_delta(v,Value(f01)-f00);
    cut.constant += f00;
  }

  void add_implication_penalty(int i, int j, Cost penalty) {
    if(penalty < 0) throw std::invalid_argument("implication penalty must be nonnegative");
    cut.add_arc(vertex(i),vertex(j),penalty);
  }

  void add_disagreement_cost(int i, int j, Cost penalty) {
    if(penalty < 0) throw std::invalid_argument("disagreement penalty must be nonnegative");
    const int u = vertex(i), v = vertex(j);
    cut.add_arc(u,v,penalty);
    cut.add_arc(v,u,penalty);
  }

  void imply(int i, int j) { cut.imply(vertex(i),vertex(j)); }

  void fix(int i, bool value) {
    const int v = vertex(i);
    if(value) cut.imply(Cut::source,v);
    else cut.imply(v,Cut::sink);
  }

  void equal(int i, int j) {
    imply(i,j);
    imply(j,i);
  }

  void add_all_true_profit(std::span<const int> variables, Cost profit) { group_profit(variables,profit,true); }
  void add_all_false_profit(std::span<const int> variables, Cost profit) { group_profit(variables,profit,false); }
  void add_all_true_profit(std::initializer_list<int> variables, Cost profit) { group_profit({variables.begin(),variables.size()},profit,true); }
  void add_all_false_profit(std::initializer_list<int> variables, Cost profit) { group_profit({variables.begin(),variables.size()},profit,false); }

  std::optional<Result> solve() const { return solve_impl(false); }
  std::optional<Result> solve_push_relabel() const { return solve_impl(true); }

private:
  std::optional<Result> solve_impl(bool push_relabel) const {
    auto answer = cut.solve(push_relabel);
    if(!answer) return std::nullopt;
    auto& [cost,side] = *answer;
    return Result{cost,std::vector<bool>(side.begin()+2,side.begin()+2+n)};
  }
};

} // namespace soryuusi
