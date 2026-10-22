#include "library/utility/fastio.hpp"

#include <limits>
#include <vector>

#include "library/utility/monotone_minima.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<long long> a(n), b(m);
  for(auto& x : a) soryuusi::cin >> x;
  for(auto& x : b) soryuusi::cin >> x;
  constexpr long long inf = std::numeric_limits<long long>::max();
  auto cost = [&](int i, int j) {
    return i-j < 0 || i-j >= n ? inf : a[i-j]+b[j];
  };
  const auto answer = soryuusi::monotoneminima<long long>(n+m-1,m,cost);
  for(int i = 0; i < n+m-1; ++i) soryuusi::cout << answer[i].second << " \n"[i+1 == n+m-1];
}
