#include "library/utility/fastio.hpp"
#include <iostream>
#include "library/math/modint.hpp"
#include "library/math/modint_factorials.hpp"
using mint=soryuusi::modint;

int main(){
  int t,m,n,k;
  soryuusi::cin>>t>>m;
  mint::set_mod(m);
  soryuusi::modint_factorials<mint> mf(m > 10'000'000 ? 10'000'000 : m-1);
  while(t--){
    soryuusi::cin>>n>>k;
    soryuusi::cout<<mf.nCk<true>(n,k).val()<<'\n';
  }
}
