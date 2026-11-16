# internal/sequence_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/sequence_base.hpp)

## 機能・前提条件

平衡化方式から独立した、1ノード1要素の可変列。ノード・ルートは非公開です。コピー禁止、move可能で、cloneのみ独立複製します。splitで共有するのはarenaの所有権だけで、可変部分木は共有しません。

## インターフェース

`SequenceBase<Ops,Backend>` を使用します。Backendはtemplate<class Node,class Ops>の型です。既存のSplayTree/LazySplayTreeとTreapSequence/LazyTreapSequenceがこのクラスを利用します。

公開操作はinsert_at/erase_at/set/get/flip/flip_all/prod/prodrev/prod_all/prodrev_all、split/concat、size/empty、clone/to_vector/clear/allocated_nodes/print。作用を持つOpsのみapplyを公開します。添字は0始まり、区間は半開です。

split(k)は呼出元を空にし、先頭k要素と残りの列を返します。concat(std::move(other))は相手を空にして末尾へ連結。同一arenaでは木を結合し、異なるarenaでは値を移して結合します。clearは個別回収せず、eraseで切り離したノードは再利用します。

新しいBackendには次を実装します。

| 操作 | 契約 |
| --- | --- |
| prepare(node) | 新規・再利用ノードの平衡化情報を初期化 |
| access(root&,k) | 対象までpushしてノードを返す。必要ならrootを変更 |
| split(root,k) | 指定位置で分割し、親がnullの独立した根を返す |
| join(left,right) | inorder順を保って結合し、親がnullの根を返す |
| build(n,factory) | factory(i)で各要素を1度生成し、集約済みの根を返す |

Nodeにはpp/lp/rp、value/lsum/rsumを要求します。OpsはNode/value_type/action_type/has_action、size/identity/push/pull/toggle（作用付きならapply）を提供します。setはaccess後、対象から親へpullします。構造変更に必要な平衡化はsplit/join/buildが担当します。Extraに高さなどを保持すれば、AVL等のBackendも追加できます。

moveのnoexcept指定はBackendのmove特性に従います。Opsの演算やcallbackが例外を投げるケースの強い例外保証は提供しません。

## 計算量

1回の列操作はBackendの定数回の操作と同じオーダー。splayでは償却対数時間、treapでは期待対数時間。構築・clone・異arena移送はBackendのbuildに依存します。現行両Backendは構築 O(N)、to_vector/clone O(N)。全体集約・size/empty/flip_all/clearは O(1)。空間は共有arenaの確保数Kに対して O(K)、Backendの作業領域を加えます。

## 依存関係

- [node_pool.hpp](node_pool.md)

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)

## 性能比較

[ベンチマーク](../../../../tools/benchmarks/balanced_trees.cpp)をg++、`-std=c++20 -O2 -DNDEBUG`で実行しました。
比較元は今回の共通化前のワークツリー（前回の所有権整理を含む）です。各条件3回の中央値と最大RSS、
乱数はmt19937、seed=51245。N=50,000、Q=100,000で、構築・操作・破棄を含みます。
LCTはパス状の初期木、WaveletはB=20・値域0〜1023です。各操作の生成方法はリンク先のコードにあります。

| 条件 | 時間：変更前 → 変更後 | RSS KiB：変更前 → 変更後 | Node byte：変更前 → 変更後 |
| --- | --- | --- | --- |
| splay：反転と区間和を交互に実行 | 0.236 → 0.220秒 | 6,896 → 6,620 | 64 → 56 |
| 遅延splay：区間加算と区間和を交互に実行 | 0.234 → 0.230秒 | 8,976 → 8,472 | 96 → 88 |
| LCT：一点代入とパス和を交互に実行 | 0.088 → 0.073秒 | 6,548 → 6,572 | 56 → 56 |
| 遅延LCT：パス加算とパス和を交互に実行 | 0.159 → 0.131秒 | 8,472 → 8,172 | 96 → 88 |
| Wavelet：一点更新と区間頻度を交互に実行 | 0.865 → 0.872秒 | 5,712 → 5,612 | 56 → 56 |

Nodeサイズは上記型のコンパイラ出力から確認しました。splay通常版・LCT通常版の値型はlong long、
遅延版はpair<long long,int>、作用型はlong longです。
全条件のチェックサムは一致しました。splay・LCTは改善、Waveletは測定のばらつきと同程度の差で、
ノードサイズの増加もないため、共通化とbit列の独立実装を採用しています。
