# binary_trie

[索引](../README.md) / [実装](../../../library/data_structure/binary_trie.hpp)

## 機能・前提条件

非負B bit整数の**集合**。同じ値へのinsertは冪等で、eraseはその値を削除します（多重集合ではありません）。全体xor、昇順k番目、最小値・最大値を扱います。BはTの利用可能なbit幅以下にしてください。

全体xorは木にマスクを一つ保持します。検索ではマスクの各ビットに従って子を選び、ノードを書き換えません。空の木は挿入時に初めてノードを確保します。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。 mergeは集合和で、otherを空にします。両方の遅延xorを反映した値を統合し、ノードの所有権は混在させません。

## インターフェース

```cpp
#include "library/data_structure/binary_trie.hpp"
template<class T, int B = 60> struct Binary_Trie;
Binary_Trie();
int size() const;
void insert(T x);
void erase(T x);
void operate_xor(T x);
T kth(int k); // 0始まり、範囲外はT(-1)
T min_element(); // 空ならT(-1)
T max_element(); // 空ならT(-1)
void merge(Binary_Trie&& other);
Binary_Trie clone() const;
std::size_t allocated_nodes() const;
```

## 計算量

insert/erase/kth/min/maxは `O(B)`、sizeと全体xorは `O(1)`。確保済みノード数Kに対して空間 `O(K)`、cloneは `O(K)`。mergeは消費元の到達ノード数をMとして `O(M)` 時間、最大 `O(M)` ノード追加。受取側に根がなければ消費元のプールを移し、受取側の旧領域があればその解放時間を要します。削除済みノードはプールに保持します。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)

## Verify・利用例

- [`unit/sparse_structures`](../../../verify/unit/sparse_structures.cpp)
- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/set_xor_min](../../../verify/library_checker/data_structure/set_xor_min.cpp)
