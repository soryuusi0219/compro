#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/math/mod_sqrt.hpp"

int main() {
    int q;
    soryuusi::cin >> q;
    while (q--) {
        long long y, p;
        soryuusi::cin >> y >> p;
        soryuusi::cout << soryuusi::mod_sqrt(y, p) << '\n';
    }
}
