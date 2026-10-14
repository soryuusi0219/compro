#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/math/prime_table.hpp"

int main() {
    int n,a,b;
    soryuusi::cin >> n >> a >> b;
    auto PT = soryuusi::PrimeTable(n+1);
    while(PT.size() && PT.back() > n) PT.pop_back();
    int x = (PT.size()-1-b) / a + 1;
    soryuusi::cout << PT.size() << ' ' << x << '\n';
    for(int i = 0; i < x; ++i) soryuusi::cout << PT[a*i+b] << ' ';
    soryuusi::cout << '\n';
}
