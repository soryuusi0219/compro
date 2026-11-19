# internal/sortable_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/sortable_base.hpp)

## 機能・前提条件

ソート可能列の共通基盤です。外側はソート済み区間を並べる重み付きSequence、内側はキー順のExplicit木です。
各区間の内側の木をCompare順で保持し、外側のノードに区間の向きを持ちます。
ノードの確保・再利用はTreeArena、回転・分割・結合・構築は既存のBackend、内側のmeldはExplicitSetを使います。

## インターフェース

`SortableArena<Key,Ops,Backend,Balance,Compare>` が内外のarenaを作り、new_sequenceで
`SortableSequence` を生成します。公開操作は [splay_sortable](../splay_sortable.md) /
[treap_sortable](../treap_sortable.md) を参照してください。

`SortableBlockOps` は内側のサイズを重み、内側の順逆集約を外側ノードの値として使います。
重みの途中で分割する場合は、外側で区間を切り離してから内側を順位で分割し、2区間を付け直します。
読み取りの区間集約では区間を切り離さず、外側と内側の保存済み集約を使います。範囲が区間の途中から始まる、または終わる場合だけ、境界区間の内側の木を探索します。
外側の反転は子と順逆集約を交換し、自区間の向きも反転します。減少方向のキー作用では
内側の木と区間の向きを両方反転するため、列順が保たれます。値タグ・キータグ・列反転は独立に伝播します。

要素参照は内側のノードに属します。各区間の内側の根から外側ノードを特定して外側での区間開始位置を
求め、区間内順位を向きに合わせて足します。内側の根が変わる操作ではこの対応を付け直します。
callbackから同じarenaへ再入することや、同じarenaへの並行操作は想定しません。
コールバックが例外を投げた場合の復元は行いません。

## 計算量

内外で通常の木操作を定数回ずつ行います。ソートでは外側の対象区間を走査し、内側をmeldごとに併合します。
詳細な計算量は公開型のreferenceを参照してください。

## 依存関係

- [binary_tree_ops.hpp](binary_tree_ops.md)
- [explicit_base.hpp](explicit_base.md)
- [tree_arena.hpp](tree_arena.md)

## Verify・利用例

- [unit/sortable_trees](../../../../verify/unit/sortable_trees.cpp)
- [library_checker/point_set_range_sort_range_composite_splay](../../../../verify/library_checker/data_structure/point_set_range_sort_range_composite_splay.cpp)
- [library_checker/point_set_range_sort_range_composite_treap](../../../../verify/library_checker/data_structure/point_set_range_sort_range_composite_treap.cpp)

## 性能比較

公式の `max_random_02` と `sort_only_short_00` で、各3回、g++の
`-std=c++20 -O2 -DNDEBUG` を使って単要素区間のmeld専用経路を比較しました。
表は共通meld → 専用経路の時間中央値と最大RSSです。

| 入力 | Backend | 時間 秒 | 最大RSS KiB |
| --- | --- | ---: | ---: |
| max_random_02 | splay | 2.55 → 2.87 | 18,044 → 18,064 |
| max_random_02 | treap | 2.22 → 2.16 | 20,604 → 20,768 |
| sort_only_short_00 | splay | 0.42 → 0.41 | 17,568 → 17,592 |
| sort_only_short_00 | treap | 0.26 → 0.26 | 19,776 → 19,776 |

出力は一致しました。専用経路による一貫した短縮が見られなかったため、共通meldを採用しました。
