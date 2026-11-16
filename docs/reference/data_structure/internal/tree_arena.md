# internal/tree_arena

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/tree_arena.hpp)

## 機能・前提条件

可変木のノードプール・再利用リスト・平衡化Backendをまとめる内部の所有領域です。ノードごとの参照カウントやdeleteは行いません。列・集合ハンドルはshared_ptrでarena全体を共有所有します。可変ノード自体は複数の木で共有しません。管理型のmove代入でも既存ハンドルのarenaは保持されます。これは管理型の破棄・置換で無効になる借用の永続Rootとは異なります。

## インターフェース

`TreeArena<Ops,Backend>` はNodePool、free_nodes、engineを保持します。create(args...)は新規または再利用ノードを構築し、engine.prepareを実行します。release(node)は切り離し済みの単一ノードを再利用リストへ戻します。公開済み永続ノードには使用しません。

同一arenaの作業領域と乱数状態を共有します。同一arenaへの並行操作・演算callbackからの再入は想定しません。

## 計算量

create/releaseは償却 O(1)。A確保ノードに O(A) 空間、Backendの作業領域を加えます。領域全体の破棄は O(A)。

## 依存関係

- [node_pool.hpp](node_pool.md)

## Verify・利用例

- [unit/ordered_trees](../../../../verify/unit/ordered_trees.cpp)
