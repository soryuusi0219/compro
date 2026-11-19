#include "library/math/combinatorics.hpp"
#include "library/math/modint.hpp"
#include "library/utility/fastio.hpp"

int main(){
  using Mint = soryuusi::modint998244353;
  int t;
  soryuusi::cin >> t;
  while(t--){
    int n;
    soryuusi::cin >> n;
    soryuusi::cout << soryuusi::factorial<Mint>(n).val() << '\n';
  }
}
