# skew_heap

[索引](../README.md) / [実装](../../../library/data_structure/skew_heap.hpp)

## 機能・前提条件

最小キーの取り出し、ヒープのmeld、全キーへの一様加算を行う。値は加算の影響を受けない。
キー・値の重複を許す。同じキーを持つ要素の取り出し順は任意。
キーはゼロ、比較、等値判定、加算を持ち、一様加算で順序を保つ型を使う。キーの加算と `std::size_t` 型の論理サイズがオーバーフローしないように使う。永続版のキー・値と、両版で `top()` が返すキー・値はコピーできる型を使う。

## インターフェース

```cpp
#include "library/data_structure/skew_heap.hpp"

soryuusi::SkewHeap<long long,int> arena;
auto a = arena.new_heap();
auto b = arena.new_heap();
a.push(7,100);
b.push(2,200);
a.add_all(-8);
a.meld(std::move(b));  // b は空になり、再度 push できる
auto [key,value] = a.top();  // -1, 100
a.pop();
```

- `SkewHeap<Key,Value = std::monostate>` は `new_heap()` で可変の `Heap` を作る。管理型・ハンドルはコピー禁止、move可能。
- `Heap` の `meld(Heap&& other)` は同じarenaの異なるハンドルに対して使う。消費元は空になる。各ハンドルがarenaを保持するため、管理型の破棄後も操作できる。
- `Heap` は `push(Key key, Value value = {})`、`pop()`、`add_all(const Key& delta)`、`top() const`、`size() const`、`empty() const` を持つ。 `top()` は `std::pair<Key,Value>` を値で返す。
- `PersistentSkewHeap<Key,Value = std::monostate>` は `empty_root()` で空の版を作る。`push(root,key,value)`、`pop(root)`、`add_all(root,delta)`、`meld(a,b)` は更新後の `Root` を返し、入力版はそのまま使える。
- 永続版の `empty(root)`、`size(root)`、`top(root)` はconstでノードを確保しない。meldの2版は同じ管理領域から渡し、同一版や共有部分を持つ版のmeldでは要素の多重度を足し合わせる。
- 両版とも `top()` と `pop()` の対象は非空とする。`allocated_nodes()` は管理領域の累積確保数を返す。ノードは管理領域単位で解放する。
- 永続版の `Root` は管理領域を借用する。管理型のmove構築・move代入で移動元の版を引き継ぐ。領域の破棄やmove代入による置換で、その領域の版が無効になる。

## 計算量

サイズ、空判定、最小要素の取得、全キー加算は O(1)。可変版の挿入・削除・meldは、対象の合計サイズを (N) として償却 O(log(N+1))、単発で最悪 O(N)。挿入ごとに1ノードを確保する。

永続版の挿入・削除・meldは、meldでたどる長さを (L) として時間・確保数が O(L+1)、最悪 O(N)。同じ版から繰り返し分岐する場合は単発の上界を使う。全キー加算で確保するノードは高々1個。空版または新しい最小キーの挿入では1ノードだけを確保し、O(1) で処理します。両版ともmeldは反復処理で、木の深さに比例する呼び出しスタックを使わない。

## 依存関係

- [node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [`unit/skew_heap`](../../../verify/unit/skew_heap.cpp)
- [`library_checker/directedmst`](../../../verify/library_checker/graph/directedmst.cpp)（可変版）
