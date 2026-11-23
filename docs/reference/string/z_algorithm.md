# z_algorithm

[索引](../README.md) / [実装](../../../library/string/z_algorithm.hpp)

## 機能・前提条件

列の各suffixと全体との最長共通prefix長。string以外にも `const` で `size()` と添字アクセスができる列を渡せる。空列には空の結果を返す。

## インターフェース

```cpp
#include "library/string/z_algorithm.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class V>
vector<int> z_algo(const V& s);
```

## 計算量

O(N) 時間、出力に O(N) 領域、その他の作業領域は O(1)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/z_algorithm`](../../../verify/library_checker/string/z_algorithm.cpp)
- [`unit/string_algorithms`](../../../verify/unit/string_algorithms.cpp)
