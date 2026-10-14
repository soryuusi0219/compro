#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/math/primitive_root.hpp"

int main() {
    int q;
    long long p;
    soryuusi::cin >> q;
    while(q--){
      soryuusi::cin >> p;
      soryuusi::cout << soryuusi::primitive_root(p) << '\n';
    }
}
