#include <cassert>
#include <iostream>

#include "library/math/dirichlet_sum.hpp"

namespace {

long long divisor_count(int n) {
  long long result = 0;
  for(int d = 1; d <= n; ++d) result += n%d == 0;
  return result;
}

}  // namespace

int main() {
  using DS = soryuusi::Dirichlet_Sum<long long,0>;
  constexpr int n = 100;
  DS::init(n,10);
  const auto zeta = DS::zeta();
  const auto square = zeta*zeta;
  auto [lower,upper] = square.getAll();
  for(int i = 1; i <= 10; ++i) assert(lower[i-1] == divisor_count(i));
  long long expected = 0;
  for(int i = 1; i <= n; ++i) expected += divisor_count(i);
  assert(square.getN() == expected);
  assert((square/zeta).getN() == zeta.getN());
  assert((zeta*DS::e()).getN() == zeta.getN());
  assert(upper.front() == expected);
  std::cout << "dirichlet_sum: all tests passed\n";
}
