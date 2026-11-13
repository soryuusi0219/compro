# manacher

[索引](../README.md) / [実装](../../../library/string/manacher.hpp)

## 機能・前提条件

空でない列の各中心に対する最長回文の長さを返す。戻り値は長さ `2*N-1`。添字 `2*i` は文字i中心、`2*i+1` は文字iとi+1の間を中心とする。string版とvector版は同じ表現。

## インターフェース

```cpp
#include "library/string/manacher.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
vector<int> Manacher(const string& s);
template<typename T>
vector<int> Manacher(const vector<T>& s);
```

## 計算量

O(N) 時間・空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/enumerate_palindromes_manacher`](../../../verify/library_checker/string/enumerate_palindromes_manacher.cpp)
