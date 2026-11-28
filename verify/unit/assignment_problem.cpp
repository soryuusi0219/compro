#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "library/graph/assignment_problem.hpp"

namespace {

using Wide = __int128_t;
using ll = long long;
using Matrix = std::vector<std::vector<ll>>;
using Mask = std::vector<std::vector<bool>>;

std::optional<Wide> enumerate(const Matrix& cost, const Mask& allowed) {
  const int n = int(cost.size()), m = n ? int(cost[0].size()) : 0;
  std::optional<Wide> answer;
  auto dfs = [&](auto&& self, int i, int used, Wide value) -> void {
    if(i == n) {
      if(!answer || value < *answer) answer = value;
      return;
    }
    for(int j = 0; j < m; ++j) if(!(used>>j&1) && allowed[i][j]) {
      self(self,i+1,used|(1<<j),value+cost[i][j]);
    }
  };
  dfs(dfs,0,0,0);
  return answer;
}

void check(const Matrix& cost, const Mask& allowed, const soryuusi::AssignmentResult& result) {
  const int n = int(cost.size()), m = int(result.right_to_left.size());
  assert(result.left_to_right.size() == cost.size() && result.left_potential.size() == cost.size());
  assert(result.right_potential.size() == result.right_to_left.size());
  Wide primal = 0, dual = 0;
  for(int i = 0; i < n; ++i) {
    const int j = result.left_to_right[i];
    assert(0 <= j && j < m && allowed[i][j] && result.right_to_left[j] == i);
    assert(result.left_potential[i]+result.right_potential[j] == cost[i][j]);
    primal += cost[i][j];
    dual += result.left_potential[i];
    for(int k = 0; k < m; ++k) if(allowed[i][k]) assert(result.left_potential[i]+result.right_potential[k] <= cost[i][k]);
  }
  for(int j = 0; j < m; ++j) {
    const int i = result.right_to_left[j];
    assert(result.right_potential[j] <= 0);
    if(i < 0) assert(result.right_potential[j] == 0);
    else assert(i < n && result.left_to_right[i] == j);
    dual += result.right_potential[j];
  }
  assert(primal == result.cost && dual == primal);
}

void random_cases() {
  std::mt19937 rng(99441);
  for(int rep = 0; rep < 1700; ++rep) {
    const int n = 1+rng()%6, m = rng()%8;
    Matrix cost(n,std::vector<ll>(m));
    Mask allowed(n,std::vector<bool>(m));
    std::vector<ll> flat;
    std::vector<unsigned char> mask;
    for(int i = 0; i < n; ++i) for(int j = 0; j < m; ++j) {
      cost[i][j] = rep%5 == 0 ? -3 : int(rng()%51)-25;
      allowed[i][j] = rep%3 == 0 || rng()%4 != 0;
      flat.push_back(cost[i][j]);
      mask.push_back(allowed[i][j]);
    }
    const auto best = enumerate(cost,allowed);
    const auto nested = soryuusi::assignment_problem(cost,allowed);
    const auto linear = soryuusi::assignment_problem<ll>(n,m,flat,mask);
    assert(bool(nested) == bool(best) && bool(linear) == bool(best));
    if(nested) {
      assert(nested->cost == best && linear->cost == best);
      check(cost,allowed,*nested);
      check(cost,allowed,*linear);
      if(rep%3 == 0) {
        const auto unmasked = soryuusi::assignment_problem(cost);
        assert(unmasked && unmasked->cost == best);
        check(cost,allowed,*unmasked);
      }
    }
  }
}

void boundaries() {
  constexpr ll lo = std::numeric_limits<ll>::min(), hi = std::numeric_limits<ll>::max();
  Matrix cost{{hi,lo,hi},{-3,lo,hi}};
  Mask allowed{{true,true,true},{true,true,false}};
  const auto answer = soryuusi::assignment_problem(cost,allowed);
  assert(answer && answer->cost == Wide(lo)-3);
  check(cost,allowed,*answer);
  const auto empty = soryuusi::assignment_problem<int>(0,7,{});
  assert(empty && empty->right_to_left == std::vector<int>(7,-1));
  assert(empty->right_potential == std::vector<Wide>(7,0));
  assert(!soryuusi::assignment_problem(Matrix(3,std::vector<ll>(3,0)),{{true,false,false},{true,false,false},{true,true,true}}));
  bool rejected = false;
  try { soryuusi::assignment_problem(Matrix{{1,2},{1}}); }
  catch(const std::invalid_argument&) { rejected = true; }
  assert(rejected);

  for(int sign : {-1,1}) {
    Matrix product(64,std::vector<ll>(64));
    for(int i = 0; i < 64; ++i) for(int j = 0; j < 64; ++j) product[i][j] = sign*i*j-123*i+11*j;
    const auto ordered = soryuusi::assignment_problem(product);
    assert(ordered);
    check(product,Mask(64,std::vector<bool>(64,true)),*ordered);
  }

  std::mt19937 rng(22108);
  for(int rep = 0; rep < 35; ++rep) {
    const int n = 32+rng()%50;
    Matrix dense(n,std::vector<ll>(n));
    for(auto& row : dense) for(auto& c : row) c = int(rng()%(rep%3 == 0 ? 7 : 10001))-5000;
    const auto accelerated = soryuusi::assignment_problem(dense);
    assert(accelerated);
    check(dense,Mask(n,std::vector<bool>(n,true)),*accelerated);
  }

  Matrix all_equal(4,std::vector<ll>(80000,-hi));
  const auto rectangular = soryuusi::assignment_problem(all_equal);
  assert(rectangular && rectangular->cost == -Wide(hi)*4);
  check(all_equal,Mask(4,std::vector<bool>(80000,true)),*rectangular);
}

} // namespace

int main() {
  random_cases();
  boundaries();
  std::cout << "OK\n";
}
