# binary_trie

[索引](../README.md) / [実装](../../../library/data_structure/binary_trie.hpp)

## 機能・前提条件

非負 B bit 整数の多重集合。挿入・削除・k番目・全体XORを扱う。削除する値は存在する必要がある。

## インターフェース

```cpp
#include "library/data_structure/binary_trie.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, int B = 60>
struct Binary_Trie;
Binary_Trie();
~Binary_Trie();
int size();
void insert(T x);
void erase(T x);
void operate_xor(T x);
T kth(int k);
T min_element();
T max_element();
```

## 計算量

通常操作 O(B)、全体XOR O(1)、最大 O(NB) ノード。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/set_xor_min`](../../../verify/library_checker/data_structure/set_xor_min.cpp)
