#include "library/math/binomial_coefficient.hpp"
#include "library/utility/fastio.hpp"

int main(){
  int t,m;
  soryuusi::cin >> t >> m;
  const soryuusi::BinomialCoefficient table(m);
  while(t--){
    long long n,k;
    soryuusi::cin >> n >> k;
    soryuusi::cout << table.nCk(n,k) << '\n';
  }
}
