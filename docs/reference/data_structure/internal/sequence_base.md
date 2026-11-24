# internal/sequence_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/sequence_base.hpp)

## 機能・前提条件

Backendから独立した1ノード1要素の可変列と、その生成用arenaです。SequenceArenaが領域を作り、new_sequenceでSequenceBaseハンドルを生成します。ノードとルートは非公開。各列は独立した部分木で、所有領域だけを共有します。

## インターフェース

`SequenceArena<Ops,Backend>`：コピー不可・move可能。Sequence型、new_sequence()、new_sequence(vector)、allocated_nodes()を提供します。

`SequenceBase<Ops,Backend>`：管理型だけが構築できます。move-onlyの列ハンドルです。公開操作・所有権は各splay/treapのreferenceを参照してください。cloneは同一arenaでの独立複製です。
要素参照はTreeArenaで管理し、異arenaへのconcatではコピー先の各ノードへ参照を引き継ぎます。

Backendはtemplate<class Node,class Ops>の型で、次の操作を実装します。

| 操作 | 契約 |
| --- | --- |
| prepare(node) | 新規・再利用ノードの平衡化情報を初期化 |
| access(root&,k) | 対象までpushし、対象ノードを返す。必要ならrootを更新 |
| split(root,k) | 先頭k要素と残りを、親がnullの独立した根として返す |
| join(left,right) | inorder順に結合し、親がnullの根を返す |
| build(n,factory) | 各factory(i)を1度呼び、集約済みの木を構築 |

Nodeはpp/lp/rp、value/lsum/rsumを持ちます。OpsはNode/value_type/action_type/has_action、size/identity/push/pull/toggle、作用付きならapplyを提供します。setはaccess後に対象から親へpullします。1要素の区間積はBackendの位置アクセスで取得します。構造変更に必要な平衡化はBackendが担当します。高さなどの追加状態はPolicyのExtraに保持できます。

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

## 区間処理・meldの高速化

比較元はコミット `58d02cc` です。g++ 11.3.0、`-std=c++20 -O2 -DNDEBUG` で前後を交互に測定しました。
表は時間の中央値と最大RSSです。すべての実行でチェックサム、または出力全体が一致しました。

- [区間ベンチマーク](../../../../tools/benchmarks/balanced_trees.cpp) は50,000要素・100,000操作、乱数seed 51245です。通常列は集約と反転、遅延列は集約と区間加算を交互に行います。LCTとwaveletは同じプログラム内の既存シナリオです。
- [集合ベンチマーク](../../../../tools/benchmarks/tree_collections.cpp) は偶数・奇数キー各100,000件を作り、meld後に偶数キーを削除します。同じプログラムの多列テストは4要素の列32,768個を二分木状に32,767回連結します。
- SortableはLibrary Checkerの `point_set_range_sort_range_composite` の公式 `max_random_02` と `sort_only_short_00` を使います。どちらも100,000要素・100,000クエリです。

| 入力 | 対象 | 回数 | 時間 ms（前 → 後） | 最大RSS KiB（前 → 後） |
| --- | --- | ---: | ---: | ---: |
| 区間 | splay | 5 | 159.27 → 82.25 | 6,656 → 6,436 |
| 区間 | treap | 5 | 97.94 → 77.78 | 6,908 → 6,916 |
| 区間 | 遅延splay | 5 | 171.99 → 80.98 | 8,508 → 8,516 |
| 区間 | 遅延treap | 5 | 97.30 → 78.73 | 8,988 → 8,988 |
| LCT | 通常 | 5 | 64.41 → 62.23 | 6,588 → 6,596 |
| LCT | 遅延 | 5 | 120.44 → 117.01 | 8,188 → 8,192 |
| wavelet | 可変bit列 | 5 | 762.18 → 771.00 | 5,628 → 5,616 |
| 多列 | splay | 5 | 6.44 → 6.31 | 11,304 → 11,308 |
| 多列 | treap | 5 | 8.53 → 8.66 | 12,428 → 12,436 |
| 集合 | splay | 5 | 49.03 → 26.95 | 17,752 → 15,628 |
| 集合 | treap | 5 | 84.51 → 47.64 | 17,544 → 17,548 |
| max_random_02 | SplaySortable | 3 | 2,880 → 1,720 | 18,116 → 18,132 |
| max_random_02 | TreapSortable | 3 | 2,530 → 580 | 20,596 → 20,472 |
| sort_only_short_00 | SplaySortable | 3 | 370 → 230 | 17,680 → 17,556 |
| sort_only_short_00 | TreapSortable | 3 | 250 → 180 | 19,672 → 19,672 |

キーがint、値がlong longの通常ノードを比較すると、固定キーsplayのノードは64から56バイトに減りました。
通常列はsplayが56、treapが64、固定キーtreapは64バイトでした。Sortableの内側・外側はsplayが72・56、
treapが80・64バイトで前後同じです。

多列連結のtreapとwaveletの計測では時間がわずかに増えました。
区間操作、meld、Sortableでの短縮を確認し、探索済み経路の再利用、範囲を切断しない集約、優先度によるmeldを採用しました。
