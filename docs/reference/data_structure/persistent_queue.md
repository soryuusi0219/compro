# persistent_queue

[索引](../README.md) / [実装](../../../library/data_structure/persistent_queue.hpp)

## 機能・前提条件

任意の過去版からpush/popできる永続キュー。Rootは末尾のハンドルと要素数を持つ小さな値型で、空版も通常の値として扱います。front/popは非空版に限ります。

`Root` は不透明な借用版ハンドルです。コピーは O(1) でノードを共有しますが、公開済みノードは変更しません。更新は新しい版を返し、検索は入力版を変更せず、ノードも確保しません。管理オブジェクトはコピー禁止・move可能で、版は移動先で引き続き使用できます。管理領域の破棄・move代入による置換後は、その領域の版を使用できません。別の管理領域の版を混ぜる操作は不可で、デバッグ時には所有元を検査します。移動元の管理オブジェクトは破棄または再代入してください。 popはノードを追加しません。frontが返すconst参照はノード管理領域の存続中有効です。

## インターフェース

```cpp
#include "library/data_structure/persistent_queue.hpp"
template<class T> class Persistent_Queue;
using Root = /* opaque value */;
Root empty_root() const;
// origin: 空版の互換定数
int size(Root root) const;
bool empty(Root root) const;
const T& front(Root root) const;
Root push(Root root, const T& value);
Root push(const T& value, Root root); // 旧引数順
Root pop(Root root) const;
std::size_t allocated_nodes() const;
```

## 計算量

総push数Q。push/frontは最悪 O(log(Q+1))、pushは1ノードと O(log(Q+1)) の祖先表を追加。pop/size/empty/版コピーは O(1)。全体空間 O(Q log(Q+1))。版オブジェクト自体のヒープ確保はありません。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/persistent_queue](../../../verify/library_checker/data_structure/persistent_queue.cpp)
