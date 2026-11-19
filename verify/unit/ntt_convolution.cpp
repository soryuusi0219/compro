#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include "library/math/convolution.hpp"
#include "library/math/convolution_mod.hpp"
#include "library/math/modint.hpp"
#include "library/math/ntt.hpp"

namespace {

template <class T>
std::vector<T> brute_product(const std::vector<T>& a, const std::vector<T>& b) {
    if (a.empty() || b.empty()) return {};
    std::vector<T> result(a.size() + b.size() - 1);
    for (std::size_t i = 0; i < a.size(); ++i) {
        for (std::size_t j = 0; j < b.size(); ++j) result[i + j] += a[i] * b[j];
    }
    return result;
}

template <class Mint>
void check_transforms(std::mt19937& gen, int max_level) {
    using NTT = soryuusi::NTTEngine<Mint>;
    auto transform = [](std::vector<Mint> a) { NTT::ntt(a); return a; };
    for (int level = 0; level <= max_level; ++level) {
        const int n = 1 << level;
        std::vector<Mint> a(n);
        for (Mint& x : a) x = gen();
        const auto spectrum = transform(a);
        std::deque<Mint> portable(a.begin(), a.end());
        NTT::ntt(portable);
        assert(std::equal(portable.begin(), portable.end(), spectrum.begin(), spectrum.end()));
        NTT::intt(portable);
        assert(std::equal(portable.begin(), portable.end(), a.begin(), a.end()));
        auto copy = spectrum;
        NTT::intt_unnormalized(copy);
        for (int k = 0; k < n; ++k) assert(copy[k] == a[k] * n);
        NTT::normalize(copy.begin(), copy.end(), n);
        assert(a == copy);

        if (n <= 64) {
            for (int i = 0; i < n; ++i) {
                int j = 0;
                for (int k = 0; k < level; ++k) j = 2 * j + (i >> k & 1);
                const Mint ratio = NTT::root(level).pow(j);
                Mint value = 0, weight = 1;
                for (Mint x : a) { value += x * weight; weight *= ratio; }
                assert(value == spectrum[i]);
            }
        }

        auto expected = a;
        for (int i = 1; i < n; i += 2) expected[i] = -expected[i];
        copy = spectrum;
        NTT::ntt_negate_argument(copy);
        assert(copy == transform(expected));

        expected = a;
        std::rotate(expected.begin(), expected.end() - 1, expected.end());
        copy = spectrum;
        NTT::ntt_shift(copy);
        assert(copy == transform(expected));
        expected = a;
        std::reverse(expected.begin() + 1, expected.end());
        copy = spectrum;
        NTT::ntt_inverse_argument(copy);
        assert(copy == transform(expected));
        expected = a;
        std::reverse(expected.begin(), expected.end());
        copy = spectrum;
        NTT::ntt_reverse(copy);
        assert(copy == transform(expected));

        if (level < NTT::rank2) {
            expected = a;
            expected.resize(2 * n);
            const auto doubled = transform(expected);
            copy = spectrum;
            NTT::ntt_double(copy);
            assert(copy == doubled);
            copy = spectrum;
            NTT::ntt_odd_extend(copy);
            assert(std::equal(copy.begin(), copy.end(), doubled.begin() + n));
            std::fill(expected.begin(), expected.end(), Mint(0));
            for (int i = 0; i < n; ++i) expected[2 * i] = a[i];
            copy = spectrum;
            NTT::ntt_square_argument(copy);
            assert(copy == transform(expected));
        }
        if (n > 1) {
            std::vector<Mint> even(n / 2), odd(n / 2);
            for (int i = 0; i < n / 2; ++i) {
                even[i] = a[2 * i];
                odd[i] = a[2 * i + 1];
            }
            assert(NTT::intt_parity(spectrum, false) == even);
            assert(NTT::intt_parity(spectrum, true) == odd);
            const auto low = transform({a.begin(), a.begin() + n / 2});
            const auto high = transform({a.begin() + n / 2, a.end()});
            assert(NTT::ntt_half(spectrum, false) == low);
            assert(NTT::ntt_half(spectrum, true) == high);
            assert(NTT::ntt_split(spectrum) == std::make_pair(low, high));

            if (n <= 128) {
                expected = a;
                for (int i = 1; i < n; i += 2) expected[i] = -expected[i];
                const auto product = brute_product(a, expected);
                std::vector<Mint> folded(n / 2);
                for (int i = 0; i < (int)product.size(); i += 2) folded[i % n / 2] += product[i];
                copy = spectrum;
                NTT::ntt_even_product(copy);
                assert(copy == transform(folded));
            }
        }
    }
}

template <class Mint>
void check_convolutions(std::mt19937& gen, int n, int m) {
    std::vector<Mint> a(n), b(m);
    for (Mint& x : a) x = gen();
    for (Mint& x : b) x = gen();
    const auto product = brute_product(a, b);
    assert(soryuusi::convolution(a, b) == product);
    assert(soryuusi::convolution(b, a) == product);
    assert(soryuusi::convolution_square(b) == brute_product(b, b));
    std::vector<Mint> middle;
    if (0 < m && m <= n) middle.assign(product.begin() + m - 1, product.begin() + n);
    assert(soryuusi::middle_product(a, b) == middle);
}

}  // namespace

int main() {
    using Mint = soryuusi::modint998244353;
    using WideMint = soryuusi::static_modint<2013265921>;
    std::mt19937 gen(5846);
    check_transforms<Mint>(gen, 12);
    check_transforms<WideMint>(gen, 11);
    check_transforms<soryuusi::static_modint<2>>(gen, 0);
    for (int n : {0, 1, 16, 60, 65, 128, 129, 257, 383}) {
        for (int m : {0, 1, 31, 60, 65, 128, 168}) {
            check_convolutions<Mint>(gen, n, m);
            check_convolutions<soryuusi::static_modint<257>>(gen, n, m);
        }
    }
    check_convolutions<WideMint>(gen, 1024, 511);
    check_convolutions<Mint>(gen, 16384, 96);
    check_convolutions<Mint>(gen, 16384, 48);
    check_convolutions<soryuusi::static_modint<2>>(gen, 150, 90);
    check_convolutions<soryuusi::static_modint<10>>(gen, 125, 85);

    std::vector<long long> a(165), b(83);
    for (auto& x : a) x = static_cast<int>(gen() % 2000001) - 1000000;
    for (auto& x : b) x = static_cast<int>(gen() % 2000001) - 1000000;
    const auto product = brute_product(a, b);
    assert(soryuusi::convolution_ll(a, b) == product);
    assert(soryuusi::convolution_ll({std::numeric_limits<long long>::min()}, {1}) ==
           std::vector<long long>{std::numeric_limits<long long>::min()});
    auto normalized = product;
    for (auto& x : normalized) {
        x %= 1000000007;
        if (x < 0) x += 1000000007;
    }
    assert(soryuusi::convolution_mod(a, b) == normalized);
    for (auto& x : normalized) x = 0;
    assert(soryuusi::convolution_mod<1>(a, b) == normalized);
    using CrtMint = soryuusi::static_modint64<(1ULL << 40) + 87>;
    std::vector<CrtMint> wide_a(100), wide_b(90);
    for (auto& x : wide_a) x = (1ULL << 34) + gen();
    for (auto& x : wide_b) x = (1ULL << 34) + gen();
    assert(soryuusi::convolution_mod(wide_a, wide_b) == brute_product(wide_a, wide_b));
    wide_b.resize(30);
    assert(soryuusi::convolution_mod(wide_a, wide_b) == brute_product(wide_a, wide_b));
    const auto integer_result = soryuusi::convolution(a, b);
    assert(soryuusi::middle_product(a, b) ==
           std::vector<long long>(integer_result.begin() + b.size() - 1, integer_result.begin() + a.size()));
}
