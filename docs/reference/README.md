# ライブラリ reference

各 `library/**/*.hpp` に対応するMarkdown referenceです。機能・前提条件・インターフェース・計算量・verifyをまとめています。

## 読み方

- C++20、原則 `soryuusi` 名前空間。includeはリポジトリルートを基準にします。
- N は要素数、V/E は頂点数/辺数、Q は操作・質問数、U は座標領域幅、B はbit幅です。各ページの記号が優先します。
- 計算量は特記のない限りスカラー演算・モノイド演算・callbackを O(1) として数えます。期待値・償却・保守的上界の区別を確認してください。
- 添字・区間・構築順・単位元はヘッダーとverifyの呼出しに従います。全てのヘッダーが同じ規約ではありません。
- 宣言一覧には補助型・補助関数も含みます。詳細なオーバーロード・型制約はリンク先の実装、利用例はverifyを参照してください。
- verifyリンクは登録・使用箇所であり、全機能の検証や常時成功を保証するものではありません。

## 更新方法

ヘッダー追加・API変更時は対応ページとこの索引を更新します。本文は手で管理し、機械生成で上書きしません。`python3 tools/check_references.py` で網羅性・必須節・ローカルリンクを確認できます。`make check` にも組み込まれています。

旧 `*_document.txt` の内容は対応ページの「詳細な使用方法」に統合しました。

## data_structure

| ヘッダー / reference | 機能 |
| --- | --- |
| [Segtree_dual.hpp](data_structure/Segtree_dual.md) | 区間への作用と一点取得を行う双対セグメント木。 |
| [area_union_rectangle.hpp](data_structure/area_union_rectangle.md) | 軸平行長方形の和集合面積。 |
| [binary_trie.hpp](data_structure/binary_trie.md) | 非負 B bit 整数の多重集合。 |
| [bit.hpp](data_structure/bit.md) | 加法群上のFenwick tree。 |
| [bit_2d.hpp](data_structure/bit_2d.md) | 二次元Fenwick tree。 |
| [bit_sparse.hpp](data_structure/bit_sparse.md) | 更新座標を先に登録する疎Fenwick tree。 |
| [bit_vector.hpp](data_structure/bit_vector.md) | XOR線形基底への挿入関数 f(base,a)。 |
| [cartesian_tree.hpp](data_structure/cartesian_tree.md) | 数列からCartesian treeを構築する。 |
| [dsu.hpp](data_structure/dsu.md) | 連結成分の併合・代表元・サイズ。 |
| [dsu_undo.hpp](data_structure/dsu_undo.md) | 巻き戻し可能なUnion-Find。 |
| [dsu_undo_with_data.hpp](data_structure/dsu_undo_with_data.md) | 成分データ付き巻き戻しUnion-Find。 |
| [dynamic_dsu.hpp](data_structure/dynamic_dsu.md) | 疎な頂点IDをハッシュ表で管理するUnion-Find。 |
| [dynamic_lazy_segtree.hpp](data_structure/dynamic_lazy_segtree.md) | 巨大な整数添字区間の動的遅延セグメント木。 |
| [dynamic_segtree.hpp](data_structure/dynamic_segtree.md) | 必要なノードだけ生成する一点更新・区間集約木。 |
| [dynamic_segtree_2d.hpp](data_structure/dynamic_segtree_2d.md) | 巨大な整数座標上で一点 set/add と長方形 prod を扱う動的二次元木。 |
| [dynamic_wavelet.hpp](data_structure/dynamic_wavelet.md) | 挿入・削除できるwavelet matrix。 |
| [hash_table.hpp](data_structure/hash_table.md) | 固定容量の開番地ハッシュ表。 |
| [increment_decrement_ordered_multi_set.hpp](data_structure/increment_decrement_ordered_multi_set.md) | 各要素を±1して順位順を保つ有界整数多重集合。 |
| [interval_heap.hpp](data_structure/interval_heap.md) | 最小・最大の双方を取り出せるヒープ。 |
| [kd_tree.hpp](data_structure/kd_tree.md) | 固定した二次元点集合の一点更新・長方形集約。 |
| [lazy_kd_tree.hpp](data_structure/lazy_kd_tree.md) | 固定点集合に長方形作用と集約を行うKD木。 |
| [lazy_link_cut.hpp](data_structure/lazy_link_cut.md) | 動的森のlink/cut、パス集約とパスへの遅延作用。 |
| [lazy_segtree.hpp](data_structure/lazy_segtree.md) | モノイドと作用を受け取る遅延セグメント木。 |
| [lazy_segtree_template.hpp](data_structure/lazy_segtree_template.md) | 区間加算・代入・アフィン変換・XOR・反転転倒数などの定番設定。 |
| [li_chao_tree_dynamic.hpp](data_structure/li_chao_tree_dynamic.md) | 動的Li Chao treeによる直線・線分の最小値（比較関数で最大値）問い合わせ。 |
| [li_chao_tree_line.hpp](data_structure/li_chao_tree_line.md) | Li Chao treeで使う直線型 `Line<T>{a,b}` と無限大直線 `INF_Line<T,infinity>()`。 |
| [li_chao_tree_static.hpp](data_structure/li_chao_tree_static.md) | 問い合わせ座標を事前登録するLi Chao tree。 |
| [link_cut.hpp](data_structure/link_cut.md) | 動的森のlink/cut、evert、LCA、パス集約。 |
| [merge_sort_tree.hpp](data_structure/merge_sort_tree.md) | 静的配列の半開区間 `[l,r)` における個数・順序統計。 |
| [merge_sort_tree_bit.hpp](data_structure/merge_sort_tree_bit.md) | キー固定・重み更新可能なmerge sort tree。 |
| [merge_sort_tree_segtree.hpp](data_structure/merge_sort_tree_segtree.md) | キー固定・モノイド値を更新できるmerge sort tree。 |
| [merge_sort_tree_sum.hpp](data_structure/merge_sort_tree_sum.md) | 固定キー・固定重みの範囲和。 |
| [mo.hpp](data_structure/mo.md) | オフライン区間クエリを並べ替え、左右の追加・削除と回答callbackを呼ぶ。 |
| [mo_rollback.hpp](data_structure/mo_rollback.md) | 削除をrollbackで置き換えるMo法。 |
| [persistent_dsu.hpp](data_structure/persistent_dsu.md) | rootを引数に取る永続Union-Find。 |
| [persistent_dsu_with_data.hpp](data_structure/persistent_dsu_with_data.md) | 成分集約付き永続Union-Find。 |
| [persistent_dynamic_lazy_segtree.hpp](data_structure/persistent_dynamic_lazy_segtree.md) | rootで版を管理する永続遅延セグメント木。 |
| [persistent_dynamic_segtree.hpp](data_structure/persistent_dynamic_segtree.md) | 一点更新型の永続動的セグメント木。 |
| [persistent_queue.hpp](data_structure/persistent_queue.md) | 版ごとのpush/popを行う永続キュー。 |
| [persistent_vector.hpp](data_structure/persistent_vector.md) | 添字のbitを辿る永続配列。 |
| [point_set_range_count.hpp](data_structure/point_set_range_count.md) | 一点の値を変更し、区間内で指定した値の個数を数える。 |
| [range_parallel_unionfind.hpp](data_structure/range_parallel_unionfind.md) | 同じ長さの二つの区間を位置ごとに併合するUnion-Find。 |
| [range_set_range_prod.hpp](data_structure/range_set_range_prod.md) | 同じ値の区間代入とモノイド集約。 |
| [segtree.hpp](data_structure/segtree.md) | モノイド上の一点代入・区間集約・境界探索。 |
| [segtree_2d.hpp](data_structure/segtree_2d.md) | 密な二次元配列の一点更新・長方形集約。 |
| [segtree_2d_sparse.hpp](data_structure/segtree_2d_sparse.md) | 事前登録座標上で点の値を結合し長方形を集約する疎二次元セグメント木。 |
| [segtree_beats.hpp](data_structure/segtree_beats.md) | mapping が適用不能なら fail を返して再帰するSegment Tree Beats本体。 |
| [segtree_beats_templates.hpp](data_structure/segtree_beats_templates.md) | chmin/chmax/addと区間和・最小・最大のBeats設定。 |
| [segtree_sparse.hpp](data_structure/segtree_sparse.md) | 更新する座標を事前登録するセグメント木。 |
| [segtree_template.hpp](data_structure/segtree_template.md) | 和・最小最大・出現回数・ペア和・アフィン合成の定番Segtree設定。 |
| [splay_tree.hpp](data_structure/splay_tree.md) | 挿入・削除・反転と順逆の区間集約を扱う列splay木。 |
| [splay_tree_lazy.hpp](data_structure/splay_tree_lazy.md) | 列splay木に区間への遅延作用を追加する。 |
| [static_bit_vector.hpp](data_structure/static_bit_vector.md) | 固定bit列とprefix rank。 |
| [static_range_inversions.hpp](data_structure/static_range_inversions.md) | 静的配列の区間転倒数。 |
| [static_range_lis.hpp](data_structure/static_range_lis.md) | 静的配列の区間LIS。重複値は非減少として扱う。 |
| [static_range_prod.hpp](data_structure/static_range_prod.md) | 静的モノイド区間積。 |
| [static_rectangle_add_rectangle_sum.hpp](data_structure/static_rectangle_add_rectangle_sum.md) | 長方形加算と長方形和を全て登録してsolveするオフライン処理。 |
| [static_rectangle_sum.hpp](data_structure/static_rectangle_sum.md) | 重み付き点と長方形質問を登録するオフライン二次元範囲和。 |
| [swag.hpp](data_structure/swag.md) | 結合的演算の列キュー集約。 |
| [swag_deque.hpp](data_structure/swag_deque.md) | 両端の追加削除に対応するSWAG。 |
| [wavelet.hpp](data_structure/wavelet.md) | 非負 B bit 値の静的wavelet matrix。 |
| [wavelet_bit.hpp](data_structure/wavelet_bit.md) | キー固定のwavelet matrixとFenwick tree。 |
| [wavelet_segtree.hpp](data_structure/wavelet_segtree.md) | キー固定のwavelet matrixとセグメント木。 |
| [wavelet_sum.hpp](data_structure/wavelet_sum.md) | キーと重みが固定のwavelet matrix。 |
| [weighted_dsu.hpp](data_structure/weighted_dsu.md) | 群上の相対ポテンシャルを保持するUnion-Find。 |

## graph

| ヘッダー / reference | 機能 |
| --- | --- |
| [bellman_ford.hpp](graph/bellman_ford.md) | 負辺を許す単一始点最短路。 |
| [biconnected_graph.hpp](graph/biconnected_graph.md) | 無向グラフのlow-link、橋、二辺・二頂点連結成分、block-cut tree。 |
| [chromatic_number.hpp](graph/chromatic_number.md) | 部分集合DP・包除原理による彩色数。 |
| [chromatic_polynomial.hpp](graph/chromatic_polynomial.md) | 小さいグラフの彩色多項式をFormalPowerSeriesで返す。 |
| [complement_graph_connected_components.hpp](graph/complement_graph_connected_components.md) | 与えた単純無向グラフの補グラフの連結成分を求める。 |
| [dijkstra.hpp](graph/dijkstra.md) | 非負重みの最短路と経路復元。 |
| [dijkstra_01.hpp](graph/dijkstra_01.md) | 辺重みが0または1の最短路。 |
| [dijkstra_1.hpp](graph/dijkstra_1.md) | 全辺重み1の幅優先探索。 |
| [dynamic_connectivity_offline.hpp](graph/dynamic_connectivity_offline.md) | 辺追加削除時刻を登録し時間軸セグメント木とrollback DSUで連結性を処理する。 |
| [dynamic_connectivity_offline_data.hpp](graph/dynamic_connectivity_offline_data.md) | 動的連結性に成分値の加算・集約を追加したオフライン処理。 |
| [enumerate_cliques.hpp](graph/enumerate_cliques.md) | 無向単純グラフのクリークをcallbackへ列挙する。 |
| [enumerate_cliques_small_n.hpp](graph/enumerate_cliques_small_n.md) | 128bit整数で隣接を保持する小頂点数版クリーク列挙。 |
| [enumerate_triangles.hpp](graph/enumerate_triangles.md) | 次数に応じて辺を向き付けし、三角形の各頂点をcallbackへ渡す。 |
| [graph_general.hpp](graph/graph_general.md) | 木のパス、直径、重心などの補助関数。 |
| [incremental_msf.hpp](graph/incremental_msf.md) | 辺を順に加えた各prefixの最小全域森をオフライン分割統治で求める。 |
| [maximal_independent_set.hpp](graph/maximal_independent_set.md) | 名称はmaximalだが最大サイズの独立集合を探索し、頂点番号列を返す。 |
| [maximal_weighted_independent_set.hpp](graph/maximal_weighted_independent_set.md) | 頂点重み付き最大独立集合。 |
| [min_cost_flow_modified.hpp](graph/min_cost_flow_modified.md) | 限界費用関数を持つ最小費用流。 |
| [planar_graph.hpp](graph/planar_graph.md) | 交差のない直線埋め込みから面・面積を構築する。 |
| [prim.hpp](graph/prim.md) | 無向重み付きグラフの最小全域木。 |
| [scc.hpp](graph/scc.md) | 有向グラフの強連結成分分解と縮約DAG。 |
| [static_graph.hpp](graph/static_graph.md) | 辺を追加してbuildするCSR隣接配列。 |

## math

| ヘッダー / reference | 機能 |
| --- | --- |
| [binomial_coefficient_prefix_sum.hpp](math/binomial_coefficient_prefix_sum.md) | 質問 (n,k) ごとに二項係数のprefix和 Σ(i=0..k) C(n,i) を一括計算する。 |
| [bitmatrix.hpp](math/bitmatrix.md) | F₂上の独立したビットパック行列。 |
| [convolution.hpp](math/convolution.md) | 静的modintまたは整数係数の通常畳み込み。 |
| [convolution_bit_and.hpp](math/convolution_bit_and.md) | bit AND畳み込み。 |
| [convolution_bit_or.hpp](math/convolution_bit_or.md) | bit OR畳み込み。 |
| [convolution_bit_xor.hpp](math/convolution_bit_xor.md) | bit XOR畳み込み。 |
| [convolution_bit_xor_general.hpp](math/convolution_bit_xor_general.md) | 2が可逆でない係数環にも対応するXOR畳み込み。 |
| [convolution_gcd_lcm.hpp](math/convolution_gcd_lcm.md) | 添字のgcd/lcmに従う畳み込み。 |
| [convolution_min_plus_concave_arbitrary.hpp](math/convolution_min_plus_concave_arbitrary.md) | 一方が凹列のmin-plus畳み込み。 |
| [convolution_min_plus_convex_arbitrary_monotone_minima.hpp](math/convolution_min_plus_convex_arbitrary_monotone_minima.md) | 一方が凸列のmin-plus畳み込みをmonotone minimaで計算する。 |
| [convolution_min_plus_convex_arbitrary_smawk.hpp](math/convolution_min_plus_convex_arbitrary_smawk.md) | 一方が凸列のmin-plus畳み込みをSMAWKで計算する。 |
| [convolution_min_plus_convex_convex.hpp](math/convolution_min_plus_convex_convex.md) | 両方が凸列のmin-plus畳み込み。 |
| [convolution_mod.hpp](math/convolution_mod.md) | 複数NTT素数とCRTによる任意mod畳み込み。 |
| [convolution_online.hpp](math/convolution_online.md) | 両数列を1項ずつpushしてその時点の畳み込み係数を返すrelaxed convolution。 |
| [convolution_plus_max.hpp](math/convolution_plus_max.md) | 通常のmax-plusではなく c[k]=Σ(i+j=k) max(a[i],b[j]) を計算する。 |
| [convolution_semi_online.hpp](math/convolution_semi_online.md) | 片方の数列を固定し、もう片方をpushして畳み込み係数を順次得る。 |
| [convolution_subset.hpp](math/convolution_subset.md) | 互いに素な部分集合分割について積を足すsubset convolution。 |
| [determinant_sparse.hpp](math/determinant_sparse.md) | 疎行列を (行,列,値) のtuple列で与えて行列式を求める。 |
| [dirichlet_sum.hpp](math/dirichlet_sum.md) | 数論関数と商集合上のprefix和を保持しDirichlet畳み込み・除算・冪を行う。 |
| [discrete_logarithm.hpp](math/discrete_logarithm.md) | 作用・逆写像・冪・hashを指定する汎用baby-step giant-step。 |
| [enumerate_quotients.hpp](math/enumerate_quotients.md) | `EnumerateQuotients<T>(N)` は正整数iに対する `floor(N/i)` の異なる値を昇順に保持する。 |
| [formal_power_series.hpp](math/formal_power_series.md) | modint向け形式的冪級数。 |
| [general_floor_sum.hpp](math/general_floor_sum.md) | floorで定まる語の非可換モノイド積をEuclid再帰で求める。 |
| [gf2.hpp](math/gf2.md) | GF(2^D)のスカラー演算。 |
| [internal_bit.hpp](math/internal_bit.md) | 整数のbit幅・2冪切上げ・末尾零・popcountなどの移植用補助。 |
| [internal_math.hpp](math/internal_math.md) | 符号付き除算の丸め、剰余、Barrett/Montgomery、mod冪、素数判定などの内部数論。 |
| [light/fps_light.hpp](math/light/fps_light.md) | 軽量なFPS実装。 |
| [light/mint_factorial_light.hpp](math/light/mint_factorial_light.md) | 型ごとの共有階乗表とnPk/nCkの軽量関数。 |
| [linear_sieve.hpp](math/linear_sieve.md) | 最小素因数と素数一覧を線形篩で構築する。 |
| [math.hpp](math/math.md) | 素数判定、原始根、mod冪・逆元、CRT、floor_sumの標準数論API。 |
| [matrix.hpp](math/matrix.md) | 体（主にmodint）の密行列。 |
| [matrix_characteristic_polynomial.hpp](math/matrix_characteristic_polynomial.md) | 密行列の特性多項式 det(xI-A) を定数項から返す。 |
| [matrix_det_arbitrary_mod.hpp](math/matrix_det_arbitrary_mod.md) | 整数行列の行列式を合成数を含む任意modで求める。 |
| [min_mod_linear.hpp](math/min_mod_linear.md) | 一次式 (a*x+b) mod m のprefix最小値と区間最小値。 |
| [mod_crt.hpp](math/mod_crt.md) | CRT APIへの互換include。 |
| [mod_sqrt.hpp](math/mod_sqrt.md) | 素数modの平方根をTonelli–Shanksで求める。 |
| [modint.hpp](math/modint.md) | 32/64bitの静的・動的modint。 |
| [modint_factorials.hpp](math/modint_factorials.md) | modintの逆元・階乗・逆階乗、順列・組合せ・Catalan・格子路。 |
| [modint_traits.hpp](math/modint_traits.md) | modint互換型とNTT対応型を検出する型traits。 |
| [nimber.hpp](math/nimber.md) | 64bit nim積に対応する体要素。 |
| [ntt.hpp](math/ntt.md) | 静的modintのNTTエンジン。 |
| [number_of_increasing_sequence.hpp](math/number_of_increasing_sequence.md) | 各位置の上限制約下の増加列を畳み込みDPで数える。 |
| [piecewise_linear_function.hpp](math/piecewise_linear_function.md) | 長さ・傾き・jumpの列で区分線形関数を表す。 |
| [prime_factorization.hpp](math/prime_factorization.md) | 64bit整数のMiller–RabinとPollard rhoによる素因数分解・約数列挙。 |
| [prime_table.hpp](math/prime_table.md) | 上限までの素数一覧を共有キャッシュで取得する。 |
| [primitive_root.hpp](math/primitive_root.md) | 素数modの原始根を探索する。 |
| [set_power_series.hpp](math/set_power_series.md) | 部分集合畳み込み上の冪級数。 |
| [stern_brocot.hpp](math/stern_brocot.md) | 正の有理数をStern–Brocot木の圧縮経路で表す。 |

## string

| ヘッダー / reference | 機能 |
| --- | --- |
| [aho_corasick.hpp](string/aho_corasick.md) | 固定文字集合の複数パターン照合automaton。 |
| [eertree.hpp](string/eertree.md) | 文字を末尾追加し異なる回文をノード化する回文木。 |
| [lyndon_factorization.hpp](string/lyndon_factorization.md) | 文字列を辞書順非増加のLyndon語列へ分解し境界位置列を返す。 |
| [manacher.hpp](string/manacher.md) | 空でない列の各中心に対する最長回文の長さを返す。 |
| [rolling_hash.hpp](string/rolling_hash.md) | 2^61-1 modの部分列hash。 |
| [sort_concatenation_min.hpp](string/sort_concatenation_min.md) | 列の連結結果が辞書順最小になるよう要素列を並べ替える。 |
| [suffix_tree.hpp](string/suffix_tree.md) | 固定文字集合またはmapを使うsuffix tree。 |
| [trie.hpp](string/trie.md) | 文字列多重集合のtrie。 |
| [z_algorithm.hpp](string/z_algorithm.md) | 列の各suffixと全体との最長共通prefix長。 |

## tree

| ヘッダー / reference | 機能 |
| --- | --- |
| [01_on_tree.hpp](tree/01_on_tree.md) | 親より後に子を並べる制約の下で、c,dによる順序依存コストを最小化する木上順序付け。 |
| [auxiliary_tree.hpp](tree/auxiliary_tree.md) | 指定頂点とそのLCAだけから補助木を構築する。 |
| [centroid_decomposition.hpp](tree/centroid_decomposition.md) | 無向木の重心分解。 |
| [euler_tour.hpp](tree/euler_tour.md) | Euler tour順の部分木集約とLCA。 |
| [heavy_light_decomposition.hpp](tree/heavy_light_decomposition.md) | Heavy-Light分解。 |
| [heavy_light_decomposition_balanced.hpp](tree/heavy_light_decomposition_balanced.md) | 静的木のパス集約を平衡化したHeavy-Light構造で保持する。 |
| [lazy_euler_tour.hpp](tree/lazy_euler_tour.md) | Euler tour上の部分木作用と集約、LCA。 |
| [lca_doubling.hpp](tree/lca_doubling.md) | 二分累乗祖先表によるLCA・距離・パス上jump。 |
| [permutation_tree.hpp](tree/permutation_tree.md) | 順列の共通区間を表す木を構築する。 |
| [rerooting.hpp](tree/rerooting.md) | 辺写像とモノイドを指定する全方位木DP。 |
| [simple_euler_tour.hpp](tree/simple_euler_tour.md) | Euler tourとRMQによるLCA・距離。 |
| [static_top_tree.hpp](tree/static_top_tree.md) | 静的木をrake/compressで分解し一点変更に伴う木DPを更新する。 |
| [static_top_tree_rerooting.hpp](tree/static_top_tree_rerooting.md) | 静的top tree上で一点変更と任意根のDP取得を行う。 |
| [steiner_tree.hpp](tree/steiner_tree.md) | 少数のterminalを結ぶ最小Steiner木。 |
| [tree_isom.hpp](tree/tree_isom.md) | 根付き・根なし木の同型判定。 |

## utility

| ヘッダー / reference | 機能 |
| --- | --- |
| [64_ary_tree.hpp](utility/64_ary_tree.md) | 有界整数集合を64分木bit列で保持する。 |
| [compress.hpp](utility/compress.md) | 値列の座標圧縮。 |
| [fastio.hpp](utility/fastio.md) | fread/fwriteによる整数・文字列・文字のバッファ入出力。 |
| [monotone_minima.hpp](utility/monotone_minima.md) | 行最小位置が単調な行列をcallbackで評価して各行の (列,値) を返す。 |
| [monotone_minima_dp.hpp](utility/monotone_minima_dp.md) | 単調最小位置を持つDP遷移の高速化。 |
| [rand.hpp](utility/rand.md) | `random_integer<Int>(lower,upper)` で閉区間 `[lower,upper]` の一様整数乱数を得る。 |
| [smawk.hpp](utility/smawk.md) | 全単調行列の各行最小をcallback評価で求めるSMAWK。 |
| [smawk_dp.hpp](utility/smawk_dp.md) | 全単調なDP遷移のSMAWK高速化。 |
| [time.hpp](utility/time.md) | tick()による区間実行時間の簡易計測。 |
