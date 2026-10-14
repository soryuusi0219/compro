#include "library/utility/fastio.hpp"
#include <cassert>
#include <iostream>
#include <vector>

#include "library/math/math.hpp"
#include "library/math/mod_crt.hpp"
#include "library/math/modint.hpp"

static_assert(soryuusi::divfloor(-7, 3) == -3);
static_assert(soryuusi::divceil(-7, 3) == -2);
static_assert(soryuusi::internal::primitive_root<998244353> == 3);
static_assert(soryuusi::is_prime(1000000007));
static_assert(soryuusi::primitive_root(998244353) == 3);
static_assert(soryuusi::pow_mod(-2, 5, 13) == 7);
static_assert(soryuusi::inv_mod(7, 13) == 2);
static_assert(soryuusi::static_modint<13>(7).inv().val() == 2);
static_assert(soryuusi::static_modint64<1000000007ULL>(2).pow(30).val() ==
              73741817);

int main() {
    assert(soryuusi::pow_mod(-2, 5, 13) == 7);
    assert(soryuusi::inv_mod(7, 13) == 2);
    assert((soryuusi::crt({2, 3, 2}, {3, 5, 7}) ==
            std::pair<long long, long long>(23, 105)));
    assert(soryuusi::modcrt({2, 3, 2}, {3, 5, 7}, 11) == 1);
    int q;
    soryuusi::cin >> q;
    while (q--) {
        long long n, m, a, b;
        soryuusi::cin >> n >> m >> a >> b;
        soryuusi::cout << soryuusi::floor_sum(n, m, a, b) << '\n';
    }
}
