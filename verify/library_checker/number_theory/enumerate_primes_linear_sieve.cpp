#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/math/linear_sieve.hpp"
#include "library/math/prime_table.hpp"

const int MX = 100'000'000;
int main() {
    int n,a,b;
    soryuusi::cin >> n >> a >> b;
    soryuusi::Linear_Sieve sieve(std::min(n,MX)+1);
    auto PT_sieve = sieve.primes;
    if(n >= MX){
      auto PT = soryuusi::PrimeTable(n+1);
      while(PT.size() && PT.back() > n) PT.pop_back();
      assert(PT_sieve.size() <= PT.size());
      for(int i = 0; i < (int)PT_sieve.size(); ++i){
        assert(PT_sieve[i] == PT[i]);
      }
      int x = (PT.size()-1-b) / a + 1;
      soryuusi::cout << PT.size() << ' ' << x << '\n';
      for(int i = 0; i < x; ++i) {
        soryuusi::cout << PT[a*i+b] << ' ';
      }
      soryuusi::cout << '\n';

    }else{
      int x = (PT_sieve.size()-1-b) / a + 1;
      soryuusi::cout << PT_sieve.size() << ' ' << x << '\n';
      for(int i = 0; i < x; ++i) soryuusi::cout << PT_sieve[a*i+b] << ' ';
      soryuusi::cout << '\n';
    }
}
