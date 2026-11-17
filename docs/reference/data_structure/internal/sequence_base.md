# internal/sequence_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/sequence_base.hpp)

## 機能・前提条件

Backendから独立した1ノード1要素の可変列と、その生成用arenaです。SequenceArenaが領域を作り、new_sequenceでSequenceBaseハンドルを生成します。ノードとルートは非公開。各列は独立した部分木で、所有領域だけを共有します。

## インターフェース

`SequenceArena<Ops,Backend>`：コピー不可・move可能。Sequence型、new_sequence()、new_sequence(vector)、allocated_nodes()を提供します。

`SequenceBase<Ops,Backend>`：管理型だけが構築できます。move-onlyの列ハンドルです。公開操作・所有権は各splay/treapのreferenceを参照してください。cloneは同一arenaでの独立複製です。

Backendはtemplate<class Node,class Ops>の型で、次の操作を実装します。

| 操作 | 契約 |
| --- | --- |
| prepare(node) | 新規・再利用ノードの平衡化情報を初期化 |
| access(root&,k) | 対象までpushし、対象ノードを返す。必要ならrootを更新 |
| split(root,k) | 先頭k要素と残りを、親がnullの独立した根として返す |
| join(left,right) | inorder順に結合し、親がnullの根を返す |
| build(n,factory) | 各factory(i)を1度呼び、集約済みの木を構築 |

Nodeはpp/lp/rp、value/lsum/rsumを持ちます。OpsはNode/value_type/action_type/has_action、size/identity/push/pull/toggle、作用付きならapplyを提供します。setはaccess後に対象から親へpullします。構造変更に必要な平衡化はBackendが担当します。高さなどの追加状態はPolicyのExtraに保持できます。

同一arenaではBackendの作業領域を共有するので、callback内から同一arenaへ再入しないでください。例外を投げる演算について強い例外保証は提供しません。

## 計算量

管理型構築・空列生成 O(1)。列操作はBackendの定数回の呼出し。現行splayは償却対数時間、treapは期待対数時間で、構築・clone・to_vectorは O(N)。保持領域は全ハンドル共通で O(A)、Backendの作業領域を加えます。

## 依存関係

- [tree_arena.hpp](tree_arena.md)

## Verify・利用例

- [unit/ordered_trees](../../../../verify/unit/ordered_trees.cpp)
- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)

## 性能比較

[ベンチマーク](../../../../tools/benchmarks/tree_collections.cpp)で、4要素の列を32,768個作り、隣接する列を二分木状に32,767回concatしました。
変更前はコミット0a3bdabの列ごとの宣言、変更後は1つのarenaからnew_sequenceで作成しています。
g++、`-std=c++20 -O2 -DNDEBUG`、各条件3回の中央値と最大RSSです。構築・結合・破棄を含みます。

| Backend | 時間：変更前 → 変更後 | RSS KiB：変更前 → 変更後 |
| --- | --- | --- |
| splay | 0.0712 → 0.00692秒 | 84,136 → 11,304 |
| treap | 0.0879 → 0.00894秒 | 98,724 → 12,424 |

両版のチェックサムは一致しました。同一arenaの列ではコピー移送が不要になり、時間・メモリとも改善するため、この管理型・ハンドル構成を採用しています。
