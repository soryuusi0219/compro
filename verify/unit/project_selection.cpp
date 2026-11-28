#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "library/graph/multi_label_project_selection.hpp"
#include "library/graph/project_selection.hpp"

namespace {

using Wide = __int128_t;
using ll = long long;
using Binary = soryuusi::ProjectSelection<>;
using Multi = soryuusi::MultiLabelProjectSelection<>;

void binary_random() {
  std::mt19937 rng(901221);
  for(int rep = 0; rep < 2200; ++rep) {
    const int n = 1+rng()%7;
    Binary model(n);
    using Evaluate = std::function<void(int,Wide&,bool&)>;
    std::vector<Evaluate> terms;
    for(int i = 0; i < 15; ++i) {
      const int u = rng()%n, v = rng()%n, cost = int(rng()%21)-10, w = rng()%21;
      const bool flag = rng()%2;
      switch(rng()%10) {
        case 0:
          model.add_cost(u,cost,flag);
          terms.push_back([=](int mask, Wide& x, bool&) { if(bool(mask>>u&1) == flag) x += cost; });
          break;
        case 1:
          model.add_profit(u,cost,flag);
          terms.push_back([=](int mask, Wide& x, bool&) { if(bool(mask>>u&1) == flag) x -= cost; });
          break;
        case 2:
          model.add_constant(cost);
          terms.push_back([=](int, Wide& x, bool&) { x += cost; });
          break;
        case 3:
          model.add_pair_cost(u,v,cost,2,3,5-cost-w);
          terms.push_back([=](int mask, Wide& x, bool&) {
            const int table[]{cost,2,3,5-cost-w};
            x += table[2*(mask>>u&1)+(mask>>v&1)];
          });
          break;
        case 4:
          model.add_implication_penalty(u,v,w);
          terms.push_back([=](int mask, Wide& x, bool&) { if((mask>>u&1) && !(mask>>v&1)) x += w; });
          break;
        case 5:
          model.add_disagreement_cost(u,v,w);
          terms.push_back([=](int mask, Wide& x, bool&) { if((mask>>u&1) != (mask>>v&1)) x += w; });
          break;
        case 6:
          model.fix(u,flag);
          terms.push_back([=](int mask, Wide&, bool& ok) { ok &= bool(mask>>u&1) == flag; });
          break;
        case 7:
          model.imply(u,v);
          terms.push_back([=](int mask, Wide&, bool& ok) { ok &= !(mask>>u&1) || (mask>>v&1); });
          break;
        case 8:
          model.equal(u,v);
          terms.push_back([=](int mask, Wide&, bool& ok) { ok &= (mask>>u&1) == (mask>>v&1); });
          break;
        default: {
          std::vector<int> group(rng()%6);
          for(int& j : group) j = rng()%n;
          if(flag) model.add_all_true_profit(group,w);
          else model.add_all_false_profit(group,w);
          terms.push_back([=](int mask, Wide& x, bool&) {
            if(std::all_of(group.begin(),group.end(),[&](int j) { return bool(mask>>j&1) == flag; })) x -= w;
          });
        }
      }
    }
    const auto evaluate = [&](int mask) -> std::optional<Wide> {
      Wide value = 0;
      bool valid = true;
      for(const auto& eval : terms) eval(mask,value,valid);
      if(!valid) return std::nullopt;
      return value;
    };
    std::optional<Wide> best;
    int minimum = 0;
    for(int mask = 0; mask < (1<<n); ++mask) {
      const auto value = evaluate(mask);
      if(!value) continue;
      if(!best || *value < *best) best = value, minimum = mask;
      else if(value == best) minimum &= mask;
    }
    const auto result = rep%2 == 0 ? model.solve() : model.solve_push_relabel();
    assert(bool(result) == bool(best));
    if(result) {
      int mask = 0;
      for(int i = 0; i < n; ++i) if(result->selected[i]) mask |= 1<<i;
      assert(result->cost == *best && evaluate(mask) == best && mask == minimum);
    }
  }
}

void multi_random() {
  std::mt19937 rng(271986);
  for(int rep = 0; rep < 1100; ++rep) {
    const int n = 1+rng()%5;
    std::vector<int> count(n);
    for(int& k : count) k = 1+rng()%5;
    Multi model(count);
    std::vector<std::vector<ll>> unary(n);
    struct Pair { int u,v; std::vector<std::vector<ll>> table; };
    std::vector<Pair> pairs;
    std::vector<std::pair<int,int>> fixed;
    const ll constant = int(rng()%41)-20;
    model.add_constant(constant);
    for(int i = 0; i < n; ++i) {
      unary[i].resize(count[i]);
      for(ll& x : unary[i]) x = int(rng()%41)-20;
      model.add_unary_cost(i,unary[i]);
    }
    for(int i = 0; i < 8; ++i) {
      const int u = rng()%n, v = rng()%n;
      if(u == v) continue;
      const int h = count[u], w = count[v];
      std::vector<std::vector<ll>> cost(h,std::vector<ll>(w));
      for(ll& x : cost[0]) x = int(rng()%31)-15;
      for(int a = 1; a < h; ++a) {
        cost[a][0] = int(rng()%31)-15;
        for(int b = 1; b < w; ++b) cost[a][b] = cost[a-1][b]+cost[a][b-1]-cost[a-1][b-1]-ll(rng()%6);
      }
      model.add_pair_cost(u,v,cost);
      pairs.push_back({u,v,std::move(cost)});
    }
    for(int i = rng()%4; i > 0; --i) {
      const int u = rng()%n, k = rng()%count[u];
      model.fix(u,k);
      fixed.emplace_back(u,k);
    }
    const auto evaluate = [&](const std::vector<int>& x) -> std::optional<Wide> {
      for(auto [u,k] : fixed) if(x[u] != k) return std::nullopt;
      Wide value = constant;
      for(int v = 0; v < n; ++v) value += unary[v][x[v]];
      for(const auto& [u,v,table] : pairs) value += table[x[u]][x[v]];
      return value;
    };
    std::optional<Wide> best;
    std::vector<int> x(n), minimal;
    const auto walk = [&](auto&& self, int i) -> void {
      if(i == n) {
        auto value = evaluate(x);
        if(!value) return;
        if(!best || value < best) best = value, minimal = x;
        else if(value == best) {
          for(int j = 0; j < n; ++j) minimal[j] = std::min(minimal[j],x[j]);
        }
        return;
      }
      for(x[i] = 0; x[i] < count[i]; ++x[i]) self(self,i+1);
    };
    walk(walk,0);
    const auto result = rep%2 == 0 ? model.solve() : model.solve_dinic();
    assert(bool(result) == bool(best));
    if(result) {
      for(int i = 0; i < n; ++i) assert(0 <= result->label[i] && result->label[i] < count[i]);
      assert(result->cost == best && evaluate(result->label) == best && result->label == minimal);
    }
  }
}

void boundaries() {
  constexpr ll hi = std::numeric_limits<ll>::max(), lo = std::numeric_limits<ll>::min();
  Binary binary(2);
  binary.add_pair_cost(0,1,lo,hi,hi,lo);
  binary.fix(0,true);
  binary.fix(1,false);
  assert(binary.solve()->cost == hi);
  binary.add_cost(0,hi);
  assert(binary.solve()->cost == Wide(hi)*2);
  binary.imply(0,1);
  assert(!binary.solve() && !binary.solve_push_relabel());

  Binary degenerate;
  degenerate.add_all_true_profit({},hi);
  degenerate.add_all_false_profit({},hi);
  assert(degenerate.solve()->cost == -Wide(hi)*2);
  Binary self(1);
  self.add_profit(0,lo);
  self.add_pair_cost(0,0,lo,hi,hi,hi);
  assert(self.solve()->cost == lo);
  bool rejected = false;
  try {
    self.add_pair_cost(0,0,5,0,0,5);
  } catch(const std::invalid_argument&) {
    rejected = true;
  }
  assert(rejected && self.solve()->cost == lo);

  Multi wide({2,2,1});
  wide.add_pair_cost(0,1,{{lo,hi},{hi,lo}});
  wide.fix(0,1);
  wide.fix(1,0);
  wide.add_constant(hi);
  assert(wide.solve()->cost == Wide(hi)*2);
  rejected = false;
  try {
    wide.add_pair_cost(0,1,{{8,0},{0,8}});
  } catch(const std::invalid_argument&) {
    rejected = true;
  }
  assert(rejected && wide.solve()->cost == Wide(hi)*2);
  wide.fix(0,0);
  assert(!wide.solve());
  assert(Multi({}).solve()->label.empty());
  Multi uneven({1,100000,2});
  uneven.fix(1,99999);
  uneven.fix(0,0);
  assert(uneven.solve()->label == (std::vector<int>{0,99999,0}));

  // ARC129 E sample 1. Candidate values are distinct from their label indices.
  Multi arc129({2,2,2});
  const std::vector<std::vector<ll>> value{{1,5},{2,9},{7,8}};
  const std::vector<std::vector<ll>> price{{1,2},{3,4},{2,2}};
  const int weight[3][3]{{0,1,5},{0,0,3},{0,0,0}};
  for(int i = 0; i < 3; ++i) {
    arc129.add_unary_cost(i,price[i]);
    for(int j = i+1; j < 3; ++j) {
      std::vector<std::vector<ll>> pair(2,std::vector<ll>(2));
      for(int a = 0; a < 2; ++a) for(int b = 0; b < 2; ++b) {
        pair[a][b] = std::max(value[i][a]-value[j][b],value[j][b]-value[i][a])*weight[i][j];
      }
      arc129.add_pair_cost(i,j,pair);
    }
  }
  assert(arc129.solve()->cost == 28);
  assert(arc129.solve()->label == (std::vector<int>{1,1,0}));
}

} // namespace

int main() {
  binary_random();
  multi_random();
  boundaries();
  std::cout << "OK\n";
}
