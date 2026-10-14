// https://github.com/soryuusi0219/compro/blob/main/library/math/mod_crt.hpp
#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "library/math/math.hpp"

namespace soryuusi {

// Garner's algorithm. The moduli must be pairwise coprime.
// Returns the represented integer modulo final_mod without constructing the
// (possibly overflowing) product of all moduli.
inline long long modcrt(const std::vector<long long>& residues,
                        const std::vector<long long>& moduli,
                        long long final_mod) {
    assert(residues.size() == moduli.size());
    assert(1 <= final_mod);
    const std::size_t n = residues.size();
    std::vector<long long> mods = moduli;
    mods.push_back(final_mod);
    std::vector<long long> constants(n + 1, 0);
    std::vector<long long> coefficients(n + 1, 1);

    for (std::size_t i = 0; i < n; ++i) {
        assert(1 <= mods[i]);
        const auto eg = internal::inv_gcd(coefficients[i], mods[i]);
        assert(eg.first == 1);
        long long t = internal::safe_mod(residues[i], mods[i]) - constants[i];
        if (t < 0) t += mods[i];
        t = static_cast<long long>(static_cast<__int128_t>(t) * eg.second % mods[i]);
        for (std::size_t j = i + 1; j <= n; ++j) {
            constants[j] = static_cast<long long>(
                (constants[j] + static_cast<__int128_t>(coefficients[j]) * t) %
                mods[j]);
            coefficients[j] = static_cast<long long>(
                static_cast<__int128_t>(coefficients[j]) * mods[i] % mods[j]);
        }
    }
    return constants[n];
}

}  // namespace soryuusi
