# swag

[索引](../README.md) / [実装](../../../library/data_structure/swag.hpp)

## 機能・前提条件

結合的演算の列キュー集約。pushで末尾に追加し、popで先頭を削除します。要素を先頭から `x[0],...,x[n-1]` としたとき、prodは `op(x[n-1],op(...,x[0]))` の順で集約します。空のprodは単位元e()を返します。

## インターフェース

```cpp
#include "library/data_structure/swag.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T, auto op, auto e>
struct swag;
swag();
inline bool empty() const;
inline int size() const;
void p_to_s();
void push(T x);
void pop();
T prod() const;
```

## 計算量

push/popは償却 O(1)、prodは O(1)、空間は O(N)。先頭側が空のときのpopと `p_to_s()` は末尾側を移すため、単発で最悪 O(N) です。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/queue_operate_all_composite`](../../../verify/library_checker/data_structure/queue_operate_all_composite.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
