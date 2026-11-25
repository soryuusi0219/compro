# sort_concatenation_min

[索引](../README.md) / [実装](../../../library/string/sort_concatenation_min.hpp)

## 機能・前提条件

列の連結結果が辞書順最小になるよう要素列を並べ替える。空の要素も扱う。

## インターフェース

```cpp
#include "library/string/sort_concatenation_min.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
void sort_concatenation_min(std::vector<T>& v);
```

## 計算量

列数を $N$、最大長を $L$、全長を $S$ として $O(S+NL\log(N+1))$ 時間、$O(N+S)$ 補助領域。比較する片方が他方のprefixになった場合に限り、必要なZ配列を作って再利用する。

## 依存関係

- [z_algorithm.hpp](z_algorithm.md)

## Verify・利用例

- [`unit/sort_concatenation_min`](../../../verify/unit/sort_concatenation_min.cpp)
