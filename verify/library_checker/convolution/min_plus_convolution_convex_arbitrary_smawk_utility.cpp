#include "library/utility/fastio.hpp"

#include <vector>

#include "library/utility/smawk.hpp"

int main() {
  int n, m;
  soryuusi::cin >> n >> m;
  std::vector<long long> a(n), b(m);
  for(auto& x : a) soryuusi::cin >> x;
  for(auto& x : b) soryuusi::cin >> x;
  constexpr long long penalty = 1LL << 32;
  auto cost = [&](int i, int j) {
    // Extend the convex sequence with slopes -penalty and +penalty.
    int k = i-j;
    if(k < 0) return a.front()-penalty*k+b[j];
    if(k >= n) return a.back()+penalty*(k-n+1)+b[j];
    return a[k]+b[j];
  };
  const auto answer = soryuusi::SMAWK<long long>(n+m-1,m,cost);
  for(int i = 0; i < n+m-1; ++i) soryuusi::cout << answer[i].second << " \n"[i+1 == n+m-1];
}
