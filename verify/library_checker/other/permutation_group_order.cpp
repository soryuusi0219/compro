#include <vector>

#include "library/math/permutation_subgroup.hpp"
#include "library/utility/fastio.hpp"

int main(){
  constexpr int mod = 998244353;
  int n, k;
  soryuusi::cin >> n >> k;
  soryuusi::permutation_subgroup group(n);
  std::vector<int> p(n);
  while(k--){
    for(int& image : p) soryuusi::cin >> image;
    group.add(p);
  }
  group.build();
  int answer = 1;
  for(int index : group.indices()) answer = 1LL*answer*index%mod;
  soryuusi::cout << answer << '\n';
}
