#include "library/utility/fastio.hpp"
#include <cstdint>
#include <iostream>

#include "library/math/internal_math.hpp"

int main() {
    int q;
    soryuusi::cin >> q;
    while (q--) {
        std::uint64_t n;
        soryuusi::cin >> n;
        soryuusi::cout << (soryuusi::internal::is_prime64_constexpr(n) ? "Yes" : "No")
                  << '\n';
    }
}
