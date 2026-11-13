# fastio

[索引](../README.md) / [実装](../../../library/utility/fastio.hpp)

## 機能・前提条件

fread/fwriteによる整数・文字列・文字のバッファ入出力。soryuusi::cin/coutを使い、必要時にflushする。

## インターフェース

```cpp
#include "library/utility/fastio.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<unsigned int BUF_SIZE = (1U << 16)>
class Instream;
Instream();
void skipSpace();
template<typename UINT>
UINT nextUint();
template<typename INT>
INT nextInt();
std::string nextToken();
char nextChar();
Instream& operator>>(unsigned int& dest);
Instream& operator>>(int& dest);
Instream& operator>>(unsigned long& dest);
Instream& operator>>(long& dest);
Instream& operator>>(unsigned long long& dest);
Instream& operator>>(long long& dest);
Instream& operator>>(__uint128_t& dest);
Instream& operator>>(__int128_t& dest);
Instream& operator>>(std::string& dest);
Instream& operator>>(char& dest);
template<unsigned int BUF_SIZE = (1U << 16)>
class Outstream;
Outstream() = default;
~Outstream();
void write(bool flush = false);
void nextU32(u32 x);
void nextU64(u64 x);
void fixed19(u64 x);
void nextI32(i32 x);
void nextI64(i64 x);
void nextU128(u128 x);
void nextI128(i128 x);
void nextCstr(const char* x);
void nextChar(char x);
Outstream& operator<<(unsigned int targ);
Outstream& operator<<(int targ);
Outstream& operator<<(unsigned long targ);
Outstream& operator<<(long targ);
Outstream& operator<<(unsigned long long targ);
Outstream& operator<<(long long targ);
Outstream& operator<<(__uint128_t targ);
Outstream& operator<<(__int128_t targ);
Outstream& operator<<(const string& targ);
Outstream& operator<<(const char* targ);
Outstream& operator<<(char targ);
Outstream& operator<<(std::ostream& (*pf)(std::ostream&));
```

## 計算量

入出力文字数Lに O(L)、ストリームごと O(BUF_SIZE) 空間。整数変換は桁数に比例。

## 依存関係

標準ライブラリのみ。

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
- [`library_checker/min_plus_convolution_concave_arbitrary`](../../../verify/library_checker/convolution/min_plus_convolution_concave_arbitrary.cpp)
- [`library_checker/min_plus_convolution_convex_arbitrary_monotone_minima`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_monotone_minima.cpp)
- [`library_checker/min_plus_convolution_convex_arbitrary_monotone_minima_utility`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_monotone_minima_utility.cpp)
- [`library_checker/min_plus_convolution_convex_arbitrary_smawk`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_smawk.cpp)
- [`library_checker/min_plus_convolution_convex_arbitrary_smawk_utility`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_smawk_utility.cpp)
- [`library_checker/min_plus_convolution_convex_convex`](../../../verify/library_checker/convolution/min_plus_convolution_convex_convex.cpp)
- [`library_checker/area_of_union_of_rectangles`](../../../verify/library_checker/data_structure/area_of_union_of_rectangles.cpp)
- [`library_checker/associative_array`](../../../verify/library_checker/data_structure/associative_array.cpp)
- [`library_checker/deque_operate_all_composite`](../../../verify/library_checker/data_structure/deque_operate_all_composite.cpp)
- [`library_checker/double_ended_priority_queue`](../../../verify/library_checker/data_structure/double_ended_priority_queue.cpp)
- [`library_checker/dynamic_point_set_rectangle_affine_rectangle_sum`](../../../verify/library_checker/data_structure/dynamic_point_set_rectangle_affine_rectangle_sum.cpp)
- [`library_checker/dynamic_sequence_range_affine_range_sum_splay_tree`](../../../verify/library_checker/data_structure/dynamic_sequence_range_affine_range_sum_splay_tree.cpp)
- [`library_checker/line_add_get_min_dynamic`](../../../verify/library_checker/data_structure/line_add_get_min_dynamic.cpp)
- [`library_checker/line_add_get_min_static`](../../../verify/library_checker/data_structure/line_add_get_min_static.cpp)
- [`library_checker/majority_voting`](../../../verify/library_checker/data_structure/majority_voting.cpp)
- [`library_checker/ordered_set_dynamic_segment_tree`](../../../verify/library_checker/data_structure/ordered_set_dynamic_segment_tree.cpp)
- [`library_checker/ordered_set_persistent_dynamic_segment_tree`](../../../verify/library_checker/data_structure/ordered_set_persistent_dynamic_segment_tree.cpp)
- [`library_checker/persistent_queue`](../../../verify/library_checker/data_structure/persistent_queue.cpp)
- [`library_checker/persistent_range_affine_range_sum`](../../../verify/library_checker/data_structure/persistent_range_affine_range_sum.cpp)
- [`library_checker/persistent_unionfind`](../../../verify/library_checker/data_structure/persistent_unionfind.cpp)
- [`library_checker/persistent_unionfind_with_data`](../../../verify/library_checker/data_structure/persistent_unionfind_with_data.cpp)
- [`library_checker/point_add_range_sum`](../../../verify/library_checker/data_structure/point_add_range_sum.cpp)
- [`library_checker/point_add_range_sum_bit_sparse`](../../../verify/library_checker/data_structure/point_add_range_sum_bit_sparse.cpp)
- [`library_checker/point_add_range_sum_segtree`](../../../verify/library_checker/data_structure/point_add_range_sum_segtree.cpp)
- [`library_checker/point_add_range_sum_segtree_sparse`](../../../verify/library_checker/data_structure/point_add_range_sum_segtree_sparse.cpp)
- [`library_checker/point_add_rectangle_sum_dynamic_segtree_2d`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_dynamic_segtree_2d.cpp)
- [`library_checker/point_add_rectangle_sum_kd_tree`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_kd_tree.cpp)
- [`library_checker/point_add_rectangle_sum_merge_sort_tree_bit`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_merge_sort_tree_bit.cpp)
- [`library_checker/point_add_rectangle_sum_segtree_2d_sparse`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_segtree_2d_sparse.cpp)
- [`library_checker/point_add_rectangle_sum_wavelet_bit`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_wavelet_bit.cpp)
- [`library_checker/point_add_rectangle_sum_wavelet_segtree`](../../../verify/library_checker/data_structure/point_add_rectangle_sum_wavelet_segtree.cpp)
- [`library_checker/point_set_range_frequency_dynamic_wavelet`](../../../verify/library_checker/data_structure/point_set_range_frequency_dynamic_wavelet.cpp)
- [`library_checker/point_set_range_frequency_merge_sort_tree_segtree`](../../../verify/library_checker/data_structure/point_set_range_frequency_merge_sort_tree_segtree.cpp)
- [`library_checker/predecessor_problem`](../../../verify/library_checker/data_structure/predecessor_problem.cpp)
- [`library_checker/queue_operate_all_composite`](../../../verify/library_checker/data_structure/queue_operate_all_composite.cpp)
- [`library_checker/range_add_range_min_lazy_segtree`](../../../verify/library_checker/data_structure/range_add_range_min_lazy_segtree.cpp)
- [`library_checker/range_add_range_min_lazy_segtree_template`](../../../verify/library_checker/data_structure/range_add_range_min_lazy_segtree_template.cpp)
- [`library_checker/range_affine_point_get`](../../../verify/library_checker/data_structure/range_affine_point_get.cpp)
- [`library_checker/range_affine_point_get_dual_segtree`](../../../verify/library_checker/data_structure/range_affine_point_get_dual_segtree.cpp)
- [`library_checker/range_affine_range_sum_large_array`](../../../verify/library_checker/data_structure/range_affine_range_sum_large_array.cpp)
- [`library_checker/range_affine_range_sum_lazy_segtree_template`](../../../verify/library_checker/data_structure/range_affine_range_sum_lazy_segtree_template.cpp)
- [`library_checker/range_chmin_chmax_add_range_sum`](../../../verify/library_checker/data_structure/range_chmin_chmax_add_range_sum.cpp)
- [`library_checker/range_kth_smallest`](../../../verify/library_checker/data_structure/range_kth_smallest.cpp)
- [`library_checker/range_kth_smallest_wavelet_legacy`](../../../verify/library_checker/data_structure/range_kth_smallest_wavelet_legacy.cpp)
- [`library_checker/range_parallel_unionfind`](../../../verify/library_checker/data_structure/range_parallel_unionfind.cpp)
- [`library_checker/range_reverse_range_sum_splay_tree`](../../../verify/library_checker/data_structure/range_reverse_range_sum_splay_tree.cpp)
- [`library_checker/range_set_range_composite`](../../../verify/library_checker/data_structure/range_set_range_composite.cpp)
- [`library_checker/range_set_range_composite_lazy_segtree`](../../../verify/library_checker/data_structure/range_set_range_composite_lazy_segtree.cpp)
- [`library_checker/rectangle_add_point_get_lazy_kd_tree`](../../../verify/library_checker/data_structure/rectangle_add_point_get_lazy_kd_tree.cpp)
- [`library_checker/rectangle_sum_static`](../../../verify/library_checker/data_structure/rectangle_sum_static.cpp)
- [`library_checker/rectangle_sum_wavelet_segtree`](../../../verify/library_checker/data_structure/rectangle_sum_wavelet_segtree.cpp)
- [`library_checker/rectangle_sum_wavelet_sum`](../../../verify/library_checker/data_structure/rectangle_sum_wavelet_sum.cpp)
- [`library_checker/segment_add_get_min_dynamic`](../../../verify/library_checker/data_structure/segment_add_get_min_dynamic.cpp)
- [`library_checker/segment_add_get_min_static`](../../../verify/library_checker/data_structure/segment_add_get_min_static.cpp)
- [`library_checker/set_xor_min`](../../../verify/library_checker/data_structure/set_xor_min.cpp)
- [`library_checker/static_range_count_distinct_merge_sort_tree`](../../../verify/library_checker/data_structure/static_range_count_distinct_merge_sort_tree.cpp)
- [`library_checker/static_range_count_distinct_mo_rollback`](../../../verify/library_checker/data_structure/static_range_count_distinct_mo_rollback.cpp)
- [`library_checker/static_range_count_distinct_offline`](../../../verify/library_checker/data_structure/static_range_count_distinct_offline.cpp)
- [`library_checker/static_range_frequency_wavelet`](../../../verify/library_checker/data_structure/static_range_frequency_wavelet.cpp)
- [`library_checker/static_range_frequency_wavelet_sum`](../../../verify/library_checker/data_structure/static_range_frequency_wavelet_sum.cpp)
- [`library_checker/static_range_inversions`](../../../verify/library_checker/data_structure/static_range_inversions.cpp)
- [`library_checker/static_range_inversions_mo`](../../../verify/library_checker/data_structure/static_range_inversions_mo.cpp)
- [`library_checker/static_range_mode_query_increment_decrement_ordered_multiset`](../../../verify/library_checker/data_structure/static_range_mode_query_increment_decrement_ordered_multiset.cpp)
- [`library_checker/static_range_sum`](../../../verify/library_checker/data_structure/static_range_sum.cpp)
- [`library_checker/static_range_sum_with_upper_bound_merge_sort_tree`](../../../verify/library_checker/data_structure/static_range_sum_with_upper_bound_merge_sort_tree.cpp)
- [`library_checker/static_range_sum_with_upper_bound_wavelet`](../../../verify/library_checker/data_structure/static_range_sum_with_upper_bound_wavelet.cpp)
- [`library_checker/static_rectangle_add_rectangle_sum`](../../../verify/library_checker/data_structure/static_rectangle_add_rectangle_sum.cpp)
- [`library_checker/staticrmq`](../../../verify/library_checker/data_structure/staticrmq.cpp)
- [`library_checker/unionfind`](../../../verify/library_checker/data_structure/unionfind.cpp)
- [`library_checker/unionfind_dynamic`](../../../verify/library_checker/data_structure/unionfind_dynamic.cpp)
- [`library_checker/unionfind_with_potential`](../../../verify/library_checker/data_structure/unionfind_with_potential.cpp)
- [`library_checker/unionfind_with_potential_non_commutative_group`](../../../verify/library_checker/data_structure/unionfind_with_potential_non_commutative_group.cpp)
- [`library_checker/bell_number`](../../../verify/library_checker/enumerative_combinatorics/bell_number.cpp)
- [`library_checker/binomial_coefficient_prime_mod`](../../../verify/library_checker/enumerative_combinatorics/binomial_coefficient_prime_mod.cpp)
- [`library_checker/montmort_number_mod`](../../../verify/library_checker/enumerative_combinatorics/montmort_number_mod.cpp)
- [`library_checker/number_of_subsequences`](../../../verify/library_checker/enumerative_combinatorics/number_of_subsequences.cpp)
- [`library_checker/partition_function`](../../../verify/library_checker/enumerative_combinatorics/partition_function.cpp)
- [`library_checker/sharp_p_subset_sum`](../../../verify/library_checker/enumerative_combinatorics/sharp_p_subset_sum.cpp)
- [`library_checker/biconnected_components`](../../../verify/library_checker/graph/biconnected_components.cpp)
- [`library_checker/biconnected_components_block_cut_tree`](../../../verify/library_checker/graph/biconnected_components_block_cut_tree.cpp)
- [`library_checker/chromatic_number`](../../../verify/library_checker/graph/chromatic_number.cpp)
- [`library_checker/chromatic_polynomial`](../../../verify/library_checker/graph/chromatic_polynomial.cpp)
- [`library_checker/connected_components_of_complement_graph`](../../../verify/library_checker/graph/connected_components_of_complement_graph.cpp)
- [`library_checker/dynamic_graph_vertex_add_component_sum`](../../../verify/library_checker/graph/dynamic_graph_vertex_add_component_sum.cpp)
- [`library_checker/enumerate_cliques`](../../../verify/library_checker/graph/enumerate_cliques.cpp)
- [`library_checker/enumerate_cliques_small_n`](../../../verify/library_checker/graph/enumerate_cliques_small_n.cpp)
- [`library_checker/enumerate_triangles`](../../../verify/library_checker/graph/enumerate_triangles.cpp)
- [`library_checker/incremental_minimum_spanning_forest_incremental_msf`](../../../verify/library_checker/graph/incremental_minimum_spanning_forest_incremental_msf.cpp)
- [`library_checker/maximum_independent_set`](../../../verify/library_checker/graph/maximum_independent_set.cpp)
- [`library_checker/minimum_spanning_tree`](../../../verify/library_checker/graph/minimum_spanning_tree.cpp)
- [`library_checker/minimum_steiner_tree`](../../../verify/library_checker/graph/minimum_steiner_tree.cpp)
- [`library_checker/scc`](../../../verify/library_checker/graph/scc.cpp)
- [`library_checker/shortest_path`](../../../verify/library_checker/graph/shortest_path.cpp)
- [`library_checker/shortest_path_static_graph`](../../../verify/library_checker/graph/shortest_path_static_graph.cpp)
- [`library_checker/two_edge_connected_components`](../../../verify/library_checker/graph/two_edge_connected_components.cpp)
- [`library_checker/two_edge_connected_components_compressed`](../../../verify/library_checker/graph/two_edge_connected_components_compressed.cpp)
- [`library_checker/adjugate_matrix_matrix`](../../../verify/library_checker/linear_algebra/adjugate_matrix_matrix.cpp)
- [`library_checker/characteristic_polynomial_matrix`](../../../verify/library_checker/linear_algebra/characteristic_polynomial_matrix.cpp)
- [`library_checker/inverse_matrix_matrix`](../../../verify/library_checker/linear_algebra/inverse_matrix_matrix.cpp)
- [`library_checker/inverse_matrix_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/inverse_matrix_mod_2_bitmatrix.cpp)
- [`library_checker/matrix_det_arbitrary_mod_matrix`](../../../verify/library_checker/linear_algebra/matrix_det_arbitrary_mod_matrix.cpp)
- [`library_checker/matrix_det_matrix`](../../../verify/library_checker/linear_algebra/matrix_det_matrix.cpp)
- [`library_checker/matrix_det_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/matrix_det_mod_2_bitmatrix.cpp)
- [`library_checker/matrix_product_matrix`](../../../verify/library_checker/linear_algebra/matrix_product_matrix.cpp)
- [`library_checker/matrix_product_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/matrix_product_mod_2_bitmatrix.cpp)
- [`library_checker/matrix_rank_matrix`](../../../verify/library_checker/linear_algebra/matrix_rank_matrix.cpp)
- [`library_checker/matrix_rank_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/matrix_rank_mod_2_bitmatrix.cpp)
- [`library_checker/pow_of_matrix_matrix`](../../../verify/library_checker/linear_algebra/pow_of_matrix_matrix.cpp)
- [`library_checker/sparse_matrix_det`](../../../verify/library_checker/linear_algebra/sparse_matrix_det.cpp)
- [`library_checker/system_of_linear_equations_matrix`](../../../verify/library_checker/linear_algebra/system_of_linear_equations_matrix.cpp)
- [`library_checker/system_of_linear_equations_mod_2_bitmatrix`](../../../verify/library_checker/linear_algebra/system_of_linear_equations_mod_2_bitmatrix.cpp)
- [`library_checker/bernoulli_number`](../../../verify/library_checker/number_theory/bernoulli_number.cpp)
- [`library_checker/discrete_logarithm_mod`](../../../verify/library_checker/number_theory/discrete_logarithm_mod.cpp)
- [`library_checker/enumerate_primes`](../../../verify/library_checker/number_theory/enumerate_primes.cpp)
- [`library_checker/enumerate_primes_linear_sieve`](../../../verify/library_checker/number_theory/enumerate_primes_linear_sieve.cpp)
- [`library_checker/enumerate_quotients`](../../../verify/library_checker/number_theory/enumerate_quotients.cpp)
- [`library_checker/factorize`](../../../verify/library_checker/number_theory/factorize.cpp)
- [`library_checker/min_of_mod_of_linear`](../../../verify/library_checker/number_theory/min_of_mod_of_linear.cpp)
- [`library_checker/nim_product_64`](../../../verify/library_checker/number_theory/nim_product_64.cpp)
- [`library_checker/primality_test`](../../../verify/library_checker/number_theory/primality_test.cpp)
- [`library_checker/primality_test_internal_math`](../../../verify/library_checker/number_theory/primality_test_internal_math.cpp)
- [`library_checker/primitive_root`](../../../verify/library_checker/number_theory/primitive_root.cpp)
- [`library_checker/rational_approximation`](../../../verify/library_checker/number_theory/rational_approximation.cpp)
- [`library_checker/sqrt_mod`](../../../verify/library_checker/number_theory/sqrt_mod.cpp)
- [`library_checker/stern_brocot_tree`](../../../verify/library_checker/number_theory/stern_brocot_tree.cpp)
- [`library_checker/sum_of_floor_of_linear`](../../../verify/library_checker/number_theory/sum_of_floor_of_linear.cpp)
- [`library_checker/sum_of_floor_of_linear_general_floor_sum`](../../../verify/library_checker/number_theory/sum_of_floor_of_linear_general_floor_sum.cpp)
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
- [`library_checker/many_aplusb_128bit`](../../../verify/library_checker/sample/many_aplusb_128bit.cpp)
- [`library_checker/many_aplusb_signed`](../../../verify/library_checker/sample/many_aplusb_signed.cpp)
- [`library_checker/many_aplusb_unsigned`](../../../verify/library_checker/sample/many_aplusb_unsigned.cpp)
- [`library_checker/exp_of_set_power_series`](../../../verify/library_checker/set_power_series/exp_of_set_power_series.cpp)
- [`library_checker/polynomial_composite_set_power_series`](../../../verify/library_checker/set_power_series/polynomial_composite_set_power_series.cpp)
- [`library_checker/power_projection_of_set_power_series`](../../../verify/library_checker/set_power_series/power_projection_of_set_power_series.cpp)
- [`library_checker/subset_convolution`](../../../verify/library_checker/set_power_series/subset_convolution.cpp)
- [`library_checker/aho_corasick`](../../../verify/library_checker/string/aho_corasick.cpp)
- [`library_checker/eertree`](../../../verify/library_checker/string/eertree.cpp)
- [`library_checker/enumerate_palindromes_manacher`](../../../verify/library_checker/string/enumerate_palindromes_manacher.cpp)
- [`library_checker/longest_common_substring_suffix_tree`](../../../verify/library_checker/string/longest_common_substring_suffix_tree.cpp)
- [`library_checker/lyndon_factorization`](../../../verify/library_checker/string/lyndon_factorization.cpp)
- [`library_checker/number_of_substrings_suffix_tree`](../../../verify/library_checker/string/number_of_substrings_suffix_tree.cpp)
- [`library_checker/suffix_array_suffix_tree`](../../../verify/library_checker/string/suffix_array_suffix_tree.cpp)
- [`library_checker/z_algorithm`](../../../verify/library_checker/string/z_algorithm.cpp)
- [`library_checker/z_algorithm_rolling_hash`](../../../verify/library_checker/string/z_algorithm_rolling_hash.cpp)
- [`library_checker/cartesian_tree`](../../../verify/library_checker/tree/cartesian_tree.cpp)
- [`library_checker/common_interval_decomposition_tree`](../../../verify/library_checker/tree/common_interval_decomposition_tree.cpp)
- [`library_checker/dynamic_tree_vertex_add_path_sum_lazy_link_cut`](../../../verify/library_checker/tree/dynamic_tree_vertex_add_path_sum_lazy_link_cut.cpp)
- [`library_checker/dynamic_tree_vertex_add_path_sum_link_cut`](../../../verify/library_checker/tree/dynamic_tree_vertex_add_path_sum_link_cut.cpp)
- [`library_checker/dynamic_tree_vertex_set_path_composite_link_cut`](../../../verify/library_checker/tree/dynamic_tree_vertex_set_path_composite_link_cut.cpp)
- [`library_checker/jump_on_tree`](../../../verify/library_checker/tree/jump_on_tree.cpp)
- [`library_checker/jump_on_tree_heavy_light_decomposition`](../../../verify/library_checker/tree/jump_on_tree_heavy_light_decomposition.cpp)
- [`library_checker/lca`](../../../verify/library_checker/tree/lca.cpp)
- [`library_checker/lca_heavy_light_decomposition`](../../../verify/library_checker/tree/lca_heavy_light_decomposition.cpp)
- [`library_checker/lca_simple_euler_tour`](../../../verify/library_checker/tree/lca_simple_euler_tour.cpp)
- [`library_checker/point_set_tree_path_composite_sum`](../../../verify/library_checker/tree/point_set_tree_path_composite_sum.cpp)
- [`library_checker/point_set_tree_path_composite_sum_fixed_root`](../../../verify/library_checker/tree/point_set_tree_path_composite_sum_fixed_root.cpp)
- [`library_checker/rooted_tree_isomorphism_classification`](../../../verify/library_checker/tree/rooted_tree_isomorphism_classification.cpp)
- [`library_checker/rooted_tree_topological_order_with_minimum_inversions`](../../../verify/library_checker/tree/rooted_tree_topological_order_with_minimum_inversions.cpp)
- [`library_checker/tree_diameter_graph_general`](../../../verify/library_checker/tree/tree_diameter_graph_general.cpp)
- [`library_checker/tree_path_composite_sum_rerooting`](../../../verify/library_checker/tree/tree_path_composite_sum_rerooting.cpp)
- [`library_checker/vertex_add_path_sum`](../../../verify/library_checker/tree/vertex_add_path_sum.cpp)
- [`library_checker/vertex_add_subtree_sum`](../../../verify/library_checker/tree/vertex_add_subtree_sum.cpp)
- [`library_checker/vertex_add_subtree_sum_euler_tour`](../../../verify/library_checker/tree/vertex_add_subtree_sum_euler_tour.cpp)
- [`library_checker/vertex_set_path_composite`](../../../verify/library_checker/tree/vertex_set_path_composite.cpp)
- [`aoj/range_add_range_sum_lazy_segtree_template`](../../../verify/aoj/data_structure/range_add_range_sum_lazy_segtree_template.cpp)
- [`aoj/range_assign_range_min_lazy_segtree_template`](../../../verify/aoj/data_structure/range_assign_range_min_lazy_segtree_template.cpp)
- [`aoj/range_assign_range_sum_lazy_segtree_template`](../../../verify/aoj/data_structure/range_assign_range_sum_lazy_segtree_template.cpp)
- [`aoj/range_min_segtree_template`](../../../verify/aoj/data_structure/range_min_segtree_template.cpp)
- [`aoj/range_sum_bit_2d`](../../../verify/aoj/data_structure/range_sum_bit_2d.cpp)
- [`aoj/range_sum_segtree_2d`](../../../verify/aoj/data_structure/range_sum_segtree_2d.cpp)
- [`aoj/bellman_ford`](../../../verify/aoj/graph/bellman_ford.cpp)
- [`aoj/bfs_dijkstra_1`](../../../verify/aoj/graph/bfs_dijkstra_1.cpp)
- [`aoj/min_cost_flow_modified`](../../../verify/aoj/graph/min_cost_flow_modified.cpp)
- [`aoj/dictionary_trie`](../../../verify/aoj/string/dictionary_trie.cpp)
