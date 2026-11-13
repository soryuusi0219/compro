# modint

[索引](../README.md) / [実装](../../../library/math/modint.hpp)

## 機能・前提条件

32/64bitの静的・動的modint。val/raw/mod/set_mod、四則演算・pow・inv。動的mod変更後に以前の値を混用しない。

## インターフェース

```cpp
#include "library/math/modint.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
struct modint_base;
struct static_modint_base : modint_base;
struct modint64_base : modint_base;
struct static_modint64_base : modint64_base;
template <class T>
using is_signed_int = std::conjunction<std::is_integral<T>, std::is_signed<T>>;
template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;
template <class T>
using is_unsigned_int = std::conjunction<std::is_integral<T>, std::is_unsigned<T>>;
template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;
template <class T>
using is_modint = std::is_base_of<modint_base, T>;
template <class T>
using is_modint_t = std::enable_if_t<is_modint<T>::value>;
template <class T>
using is_modint64 = std::is_base_of<modint64_base, T>;
template <class T>
using is_modint64_t = std::enable_if_t<is_modint64<T>::value>;
template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base;
using mint = static_modint;
static constexpr int mod();
static constexpr mint raw(int v);
constexpr static_modint();
template <class T, internal::is_signed_int_t<T>* = nullptr>
constexpr static_modint(T v);
template <class T, internal::is_unsigned_int_t<T>* = nullptr>
constexpr static_modint(T v);
constexpr unsigned int val() const;
constexpr mint& operator++();
constexpr mint& operator--();
constexpr mint operator++(int);
constexpr mint operator--(int);
constexpr mint& operator+=(const mint& rhs);
constexpr mint& operator-=(const mint& rhs);
constexpr mint& operator*=(const mint& rhs);
constexpr mint& operator/=(const mint& rhs);
constexpr mint operator+() const;
constexpr mint operator-() const;
constexpr mint pow(long long n) const;
constexpr mint inv() const;
friend constexpr mint operator+(mint lhs, const mint& rhs);
friend constexpr mint operator-(mint lhs, const mint& rhs);
friend constexpr mint operator*(mint lhs, const mint& rhs);
friend constexpr mint operator/(mint lhs, const mint& rhs);
friend constexpr bool operator==(const mint& lhs, const mint& rhs);
friend constexpr bool operator!=(const mint& lhs, const mint& rhs);
template <int id>
struct dynamic_modint : internal::modint_base;
using mint = dynamic_modint;
static int mod();
static void set_mod(int m);
static mint raw(int v);
dynamic_modint();
template <class T, internal::is_signed_int_t<T>* = nullptr>
dynamic_modint(T v);
template <class T, internal::is_unsigned_int_t<T>* = nullptr>
dynamic_modint(T v);
unsigned int val() const;
mint& operator++();
mint& operator--();
mint operator++(int);
mint operator--(int);
mint& operator+=(const mint& rhs);
mint& operator-=(const mint& rhs);
mint& operator*=(const mint& rhs);
mint& operator/=(const mint& rhs);
mint operator+() const;
mint operator-() const;
mint pow(long long n) const;
mint inv() const;
friend mint operator+(mint lhs, const mint& rhs);
friend mint operator-(mint lhs, const mint& rhs);
friend mint operator*(mint lhs, const mint& rhs);
friend mint operator/(mint lhs, const mint& rhs);
friend bool operator==(const mint& lhs, const mint& rhs);
friend bool operator!=(const mint& lhs, const mint& rhs);
using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;
template <std::uint64_t m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint64 : internal::static_modint64_base;
using mint = static_modint64;
static constexpr std::uint64_t mod();
static constexpr mint raw(std::uint64_t v);
constexpr static_modint64();
template <class T, internal::is_signed_int_t<T>* = nullptr>
constexpr static_modint64(T v);
template <class T, internal::is_unsigned_int_t<T>* = nullptr>
constexpr static_modint64(T v);
constexpr std::uint64_t val() const;
constexpr mint& operator++();
constexpr mint& operator--();
constexpr mint operator++(int);
constexpr mint operator--(int);
constexpr mint& operator+=(const mint& rhs);
constexpr mint& operator-=(const mint& rhs);
constexpr mint& operator*=(const mint& rhs);
constexpr mint& operator/=(const mint& rhs);
constexpr mint operator+() const;
constexpr mint operator-() const;
constexpr mint pow(long long n) const;
constexpr mint inv() const;
friend constexpr mint operator+(mint lhs, const mint& rhs);
friend constexpr mint operator-(mint lhs, const mint& rhs);
friend constexpr mint operator*(mint lhs, const mint& rhs);
friend constexpr mint operator/(mint lhs, const mint& rhs);
friend constexpr bool operator==(const mint& lhs, const mint& rhs);
friend constexpr bool operator!=(const mint& lhs, const mint& rhs);
template <int id>
struct dynamic_modint64 : internal::modint64_base;
using mint = dynamic_modint64;
static std::uint64_t mod();
static void set_mod(std::uint64_t m);
static mint raw(std::uint64_t v);
dynamic_modint64();
template <class T, internal::is_signed_int_t<T>* = nullptr>
dynamic_modint64(T v);
template <class T, internal::is_unsigned_int_t<T>* = nullptr>
dynamic_modint64(T v);
std::uint64_t val() const;
mint& operator++();
mint& operator--();
mint operator++(int);
mint operator--(int);
mint& operator+=(const mint& rhs);
mint& operator-=(const mint& rhs);
mint& operator*=(const mint& rhs);
mint& operator/=(const mint& rhs);
mint operator+() const;
mint operator-() const;
mint pow(long long n) const;
mint inv() const;
friend mint operator+(mint lhs, const mint& rhs);
friend mint operator-(mint lhs, const mint& rhs);
friend mint operator*(mint lhs, const mint& rhs);
friend mint operator/(mint lhs, const mint& rhs);
friend bool operator==(const mint& lhs, const mint& rhs);
friend bool operator!=(const mint& lhs, const mint& rhs);
using modint64 = dynamic_modint64<-1>;
template <class T>
using is_static_modint = std::is_base_of<static_modint_base, T>;
template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;
template <class>
struct is_dynamic_modint : std::false_type;
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : std::true_type;
template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;
template <class T>
using is_static_modint64 = std::is_base_of<static_modint64_base, T>;
template <class T>
using is_static_modint64_t = std::enable_if_t<is_static_modint64<T>::value>;
template <class>
struct is_dynamic_modint64 : std::false_type;
template <int id>
struct is_dynamic_modint64<dynamic_modint64<id>> : std::true_type;
template <class T>
using is_dynamic_modint64_t = std::enable_if_t<is_dynamic_modint64<T>::value>;
template <class Stream, class Mint, internal::is_modint_t<Mint>* = nullptr>
Stream& operator>>(Stream& stream, Mint& value);
template <class Stream, class Mint, internal::is_modint_t<Mint>* = nullptr>
Stream& operator<<(Stream& stream, const Mint& value);
```

## 計算量

四則演算 O(1)（除算除く）、pow O(log k)、inv O(log mod)、各値 O(1) 空間。

## 依存関係

- [internal_math.hpp](internal_math.md)

## Verify・利用例

- [`library_checker/bitwise_and_convolution`](../../../verify/library_checker/convolution/bitwise_and_convolution.cpp)
- [`library_checker/bitwise_or_convolution`](../../../verify/library_checker/convolution/bitwise_or_convolution.cpp)
- [`library_checker/bitwise_xor_convolution`](../../../verify/library_checker/convolution/bitwise_xor_convolution.cpp)
- [`library_checker/bitwise_xor_convolution_general`](../../../verify/library_checker/convolution/bitwise_xor_convolution_general.cpp)
- [`library_checker/convolution_mod`](../../../verify/library_checker/convolution/convolution_mod.cpp)
- [`library_checker/convolution_mod_1000000007`](../../../verify/library_checker/convolution/convolution_mod_1000000007.cpp)
- [`library_checker/convolution_mod_1000000007_online`](../../../verify/library_checker/convolution/convolution_mod_1000000007_online.cpp)
- [`library_checker/convolution_mod_1000000007_semi_online`](../../../verify/library_checker/convolution/convolution_mod_1000000007_semi_online.cpp)
- [`library_checker/convolution_mod_large`](../../../verify/library_checker/convolution/convolution_mod_large.cpp)
- [`library_checker/convolution_mod_online`](../../../verify/library_checker/convolution/convolution_mod_online.cpp)
- [`library_checker/convolution_mod_semi_online`](../../../verify/library_checker/convolution/convolution_mod_semi_online.cpp)
- [`library_checker/gcd_convolution`](../../../verify/library_checker/convolution/gcd_convolution.cpp)
- [`library_checker/lcm_convolution`](../../../verify/library_checker/convolution/lcm_convolution.cpp)
- [`library_checker/deque_operate_all_composite`](../../../verify/library_checker/data_structure/deque_operate_all_composite.cpp)
- [`library_checker/dynamic_point_set_rectangle_affine_rectangle_sum`](../../../verify/library_checker/data_structure/dynamic_point_set_rectangle_affine_rectangle_sum.cpp)
- [`library_checker/dynamic_sequence_range_affine_range_sum_splay_tree`](../../../verify/library_checker/data_structure/dynamic_sequence_range_affine_range_sum_splay_tree.cpp)
- [`library_checker/persistent_range_affine_range_sum`](../../../verify/library_checker/data_structure/persistent_range_affine_range_sum.cpp)
- [`library_checker/queue_operate_all_composite`](../../../verify/library_checker/data_structure/queue_operate_all_composite.cpp)
- [`library_checker/range_affine_point_get`](../../../verify/library_checker/data_structure/range_affine_point_get.cpp)
- [`library_checker/range_affine_point_get_dual_segtree`](../../../verify/library_checker/data_structure/range_affine_point_get_dual_segtree.cpp)
- [`library_checker/range_affine_range_sum_large_array`](../../../verify/library_checker/data_structure/range_affine_range_sum_large_array.cpp)
- [`library_checker/range_affine_range_sum_lazy_segtree_template`](../../../verify/library_checker/data_structure/range_affine_range_sum_lazy_segtree_template.cpp)
- [`library_checker/range_parallel_unionfind`](../../../verify/library_checker/data_structure/range_parallel_unionfind.cpp)
- [`library_checker/range_set_range_composite`](../../../verify/library_checker/data_structure/range_set_range_composite.cpp)
- [`library_checker/range_set_range_composite_lazy_segtree`](../../../verify/library_checker/data_structure/range_set_range_composite_lazy_segtree.cpp)
- [`library_checker/static_rectangle_add_rectangle_sum`](../../../verify/library_checker/data_structure/static_rectangle_add_rectangle_sum.cpp)
- [`library_checker/unionfind_with_potential`](../../../verify/library_checker/data_structure/unionfind_with_potential.cpp)
- [`library_checker/unionfind_with_potential_non_commutative_group`](../../../verify/library_checker/data_structure/unionfind_with_potential_non_commutative_group.cpp)
- [`library_checker/bell_number`](../../../verify/library_checker/enumerative_combinatorics/bell_number.cpp)
- [`library_checker/binomial_coefficient_prime_mod`](../../../verify/library_checker/enumerative_combinatorics/binomial_coefficient_prime_mod.cpp)
- [`library_checker/number_of_subsequences`](../../../verify/library_checker/enumerative_combinatorics/number_of_subsequences.cpp)
- [`library_checker/partition_function`](../../../verify/library_checker/enumerative_combinatorics/partition_function.cpp)
- [`library_checker/sharp_p_subset_sum`](../../../verify/library_checker/enumerative_combinatorics/sharp_p_subset_sum.cpp)
- [`library_checker/chromatic_polynomial`](../../../verify/library_checker/graph/chromatic_polynomial.cpp)
- [`library_checker/enumerate_cliques`](../../../verify/library_checker/graph/enumerate_cliques.cpp)
- [`library_checker/enumerate_cliques_small_n`](../../../verify/library_checker/graph/enumerate_cliques_small_n.cpp)
- [`library_checker/enumerate_triangles`](../../../verify/library_checker/graph/enumerate_triangles.cpp)
- [`library_checker/adjugate_matrix_matrix`](../../../verify/library_checker/linear_algebra/adjugate_matrix_matrix.cpp)
- [`library_checker/characteristic_polynomial_matrix`](../../../verify/library_checker/linear_algebra/characteristic_polynomial_matrix.cpp)
- [`library_checker/inverse_matrix_matrix`](../../../verify/library_checker/linear_algebra/inverse_matrix_matrix.cpp)
- [`library_checker/matrix_det_matrix`](../../../verify/library_checker/linear_algebra/matrix_det_matrix.cpp)
- [`library_checker/matrix_product_matrix`](../../../verify/library_checker/linear_algebra/matrix_product_matrix.cpp)
- [`library_checker/matrix_rank_matrix`](../../../verify/library_checker/linear_algebra/matrix_rank_matrix.cpp)
- [`library_checker/pow_of_matrix_matrix`](../../../verify/library_checker/linear_algebra/pow_of_matrix_matrix.cpp)
- [`library_checker/sparse_matrix_det`](../../../verify/library_checker/linear_algebra/sparse_matrix_det.cpp)
- [`library_checker/system_of_linear_equations_matrix`](../../../verify/library_checker/linear_algebra/system_of_linear_equations_matrix.cpp)
- [`library_checker/bernoulli_number`](../../../verify/library_checker/number_theory/bernoulli_number.cpp)
- [`library_checker/discrete_logarithm_mod`](../../../verify/library_checker/number_theory/discrete_logarithm_mod.cpp)
- [`library_checker/sum_of_floor_of_linear`](../../../verify/library_checker/number_theory/sum_of_floor_of_linear.cpp)
- [`library_checker/sum_of_totient_function_dirichlet_sum`](../../../verify/library_checker/number_theory/sum_of_totient_function_dirichlet_sum.cpp)
- [`library_checker/composition_of_formal_power_series`](../../../verify/library_checker/polynomial/composition_of_formal_power_series.cpp)
- [`library_checker/composition_of_formal_power_series_large`](../../../verify/library_checker/polynomial/composition_of_formal_power_series.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series.cpp)
- [`library_checker/compositional_inverse_of_formal_power_series_large`](../../../verify/library_checker/polynomial/compositional_inverse_of_formal_power_series.cpp)
- [`library_checker/conversion_from_monomial_basis_to_newton_basis`](../../../verify/library_checker/polynomial/conversion_from_monomial_basis_to_newton_basis.cpp)
- [`library_checker/division_of_polynomials`](../../../verify/library_checker/polynomial/division_of_polynomials.cpp)
- [`library_checker/exp_of_formal_power_series`](../../../verify/library_checker/polynomial/exp_of_formal_power_series.cpp)
- [`library_checker/exp_of_formal_power_series_light`](../../../verify/library_checker/polynomial/exp_of_formal_power_series_light.cpp)
- [`library_checker/exp_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/exp_of_formal_power_series_sparse.cpp)
- [`library_checker/inv_of_formal_power_series`](../../../verify/library_checker/polynomial/inv_of_formal_power_series.cpp)
- [`library_checker/inv_of_formal_power_series_light`](../../../verify/library_checker/polynomial/inv_of_formal_power_series_light.cpp)
- [`library_checker/inv_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/inv_of_formal_power_series_sparse.cpp)
- [`library_checker/inv_of_polynomials`](../../../verify/library_checker/polynomial/inv_of_polynomials.cpp)
- [`library_checker/log_of_formal_power_series`](../../../verify/library_checker/polynomial/log_of_formal_power_series.cpp)
- [`library_checker/log_of_formal_power_series_light`](../../../verify/library_checker/polynomial/log_of_formal_power_series_light.cpp)
- [`library_checker/log_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/log_of_formal_power_series_sparse.cpp)
- [`library_checker/multipoint_evaluation`](../../../verify/library_checker/polynomial/multipoint_evaluation.cpp)
- [`library_checker/multipoint_evaluation_on_geometric_sequence`](../../../verify/library_checker/polynomial/multipoint_evaluation_on_geometric_sequence.cpp)
- [`library_checker/polynomial_interpolation`](../../../verify/library_checker/polynomial/polynomial_interpolation.cpp)
- [`library_checker/polynomial_interpolation_on_geometric_sequence`](../../../verify/library_checker/polynomial/polynomial_interpolation_on_geometric_sequence.cpp)
- [`library_checker/polynomial_root_finding`](../../../verify/library_checker/polynomial/polynomial_root_finding.cpp)
- [`library_checker/polynomial_taylor_shift`](../../../verify/library_checker/polynomial/polynomial_taylor_shift.cpp)
- [`library_checker/pow_of_formal_power_series`](../../../verify/library_checker/polynomial/pow_of_formal_power_series.cpp)
- [`library_checker/pow_of_formal_power_series_light`](../../../verify/library_checker/polynomial/pow_of_formal_power_series_light.cpp)
- [`library_checker/pow_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/pow_of_formal_power_series_sparse.cpp)
- [`library_checker/product_of_polynomial_sequence`](../../../verify/library_checker/polynomial/product_of_polynomial_sequence.cpp)
- [`library_checker/shift_of_sampling_points_of_polynomial`](../../../verify/library_checker/polynomial/shift_of_sampling_points_of_polynomial.cpp)
- [`library_checker/sqrt_of_formal_power_series`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series.cpp)
- [`library_checker/sqrt_of_formal_power_series_sparse`](../../../verify/library_checker/polynomial/sqrt_of_formal_power_series_sparse.cpp)
- [`library_checker/exp_of_set_power_series`](../../../verify/library_checker/set_power_series/exp_of_set_power_series.cpp)
- [`library_checker/polynomial_composite_set_power_series`](../../../verify/library_checker/set_power_series/polynomial_composite_set_power_series.cpp)
- [`library_checker/power_projection_of_set_power_series`](../../../verify/library_checker/set_power_series/power_projection_of_set_power_series.cpp)
- [`library_checker/subset_convolution`](../../../verify/library_checker/set_power_series/subset_convolution.cpp)
- [`library_checker/dynamic_tree_vertex_set_path_composite_link_cut`](../../../verify/library_checker/tree/dynamic_tree_vertex_set_path_composite_link_cut.cpp)
- [`library_checker/point_set_tree_path_composite_sum`](../../../verify/library_checker/tree/point_set_tree_path_composite_sum.cpp)
- [`library_checker/point_set_tree_path_composite_sum_fixed_root`](../../../verify/library_checker/tree/point_set_tree_path_composite_sum_fixed_root.cpp)
- [`library_checker/tree_path_composite_sum_rerooting`](../../../verify/library_checker/tree/tree_path_composite_sum_rerooting.cpp)
- [`library_checker/vertex_set_path_composite`](../../../verify/library_checker/tree/vertex_set_path_composite.cpp)
- [`unit/bitmatrix`](../../../verify/unit/bitmatrix.cpp)
- [`unit/math_light`](../../../verify/unit/math_light.cpp)
- [`unit/math_misc`](../../../verify/unit/math_misc.cpp)
