#include "library/utility/fastio.hpp"
#include <iostream>

#include "library/math/prime_factorization.hpp"

int main() {
    int q;
    soryuusi::cin >> q;
    while (q--) {
        long long n;
        soryuusi::cin >> n;
        const auto factors = soryuusi::prime_factorization(n);
        soryuusi::cout << factors.size();
        for (long long p : factors) soryuusi::cout << ' ' << p;
        soryuusi::cout << '\n';
    }
}
