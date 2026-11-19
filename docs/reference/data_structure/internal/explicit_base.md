# internal/explicit_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/explicit_base.hpp)

## 機能・前提条件

明示キーのordered set / multiset操作とarena管理を共通化します。キーはNodeのExtraに保持し、通常の値・集約・作用のPolicyから分離します。列と同じBackend・TreeArenaを利用します。

## インターフェース

`ExplicitArena<Key,Ops,Backend,Multi,Compare>` が領域と比較関数を所有し、new_setでExplicitSetを作ります。ExplicitMonoidはReversibleMonoidのExtraにキーと平衡化情報を追加します。キー作用を持つPolicyではExtraに独立した遅延タグを置き、pushでキー写像を子へ伝播した後に既存の反転を伝播します。減少方向では部分木を反転して順逆集約を交換します。更新した区間の両端が隣接部分木と整列していればjoinし、そうでなければ更新していない部分へmeldします。公開APIはsplay_explicit/treap_explicitのreferenceを参照してください。

順位・split/join/buildのBackend契約はSequenceBaseと共通です。キー探索は経路をpushしてから境界順位を求めます。Backendがaccess_node(root&,node)を提供すれば探索終点を直接アクセスし、なければaccess(root&,rank)を呼びます。splayでは探索終点をsplayするため償却計算量を維持します。

treapの重複可meldは優先度の高い根で相手を分割し、左右を併合します。残りの型では小さい方の中央値キーで両方を3分割し、左右を統合します。どちらも同値キーでは連結先・消費元の順を保ち、重複不可なら連結先を残します。所有権の異なる入力は、先に連結先arenaへ移送します。

## 計算量

公開型のreferenceに従います。大きい方のサイズをn、小さい方をmとします。treapの重複可meldは期待 O(m log(1+n/m))。中央値によるmeldは保守的に O(m log(n+m+1))（splayは償却、treapは期待）で、再帰深さ O(log(m+1))。
優先度による併合の参考文献は [Fast Set Operations Using Treaps](https://www.cs.cmu.edu/afs/cs.cmu.edu/project/scandal/public/papers/treaps-spaa98.html) です。

## 依存関係

- [reversible_monoid.hpp](reversible_monoid.md)
- [tree_arena.hpp](tree_arena.md)

## Verify・利用例

- [unit/ordered_trees](../../../../verify/unit/ordered_trees.cpp)

## 性能確認

[ベンチマーク](../../../../tools/benchmarks/tree_collections.cpp)で、偶数キー・奇数キーの集合を各100,000要素作り、meld後に偶数キー100,000個を削除しました。
g++、`-std=c++20 -O2 -DNDEBUG`、各3回の中央値と最大RSS、構築・操作・破棄を含みます。

| Backend | 時間 | RSS KiB |
| --- | --- | --- |
| splay | 0.0502秒 | 17,716 |
| treap | 0.0844秒 | 17,544 |

集約結果は両方とも4,999,950,000でした。十分な速度で統合・削除でき、既存の平衡化コアとノード再利用を使う設計を採用しています。
