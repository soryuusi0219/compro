# dynamic_wavelet

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_wavelet.hpp)

## 機能・前提条件

非負B bit値の列に挿入・削除・一点更新を行い、順序統計を取得するwavelet matrix。位置は0始まり、区間は半開区間、k番目は0始まりです。内部bit列には独立したDynamicBitVectorを使用します。bit圧縮とtreapの管理はそちらへ分離しています。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。 外側の列長・各段の境界・内部bit列を一体でmoveします。

## インターフェース

```cpp
#include "library/data_structure/dynamic_wavelet.hpp"
template<class T, int B = 31> struct DynamicWavelet;
DynamicWavelet(std::vector<T> values);
int size();
T get(int i);
void insert(int i, T x);
void erase(int i);
void set(int i, T x);
int rank(int l, int r, T x);
T range_kth(int l, int r, int k);
int count_lt(int l, int r, T x);
int count_interval(int l, int r, T lower, T upper);
T next(int l, int r, T x, T default_value = -1); // >=xの最小値
T prev(int l, int r, T x, T default_value = -1); // <xの最大値
DynamicWavelet clone();
```

## 計算量

現在の列長Nに対し通常操作は期待 O(B log(N+1))、構築は O(NB)、cloneは期待 O(NB log(N+1))。bit列の空ブロックは木から除去・再利用します。確保済み領域は保持するため、各段の過去最大ノード数の合計をKとすると空間は O(K) ワード。1ブロックは最大63bitで、削除により短いブロックも生じます。

## 依存関係

- [dynamic_bit_vector.hpp](dynamic_bit_vector.md)

## Verify・利用例

- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/point_set_range_frequency_dynamic_wavelet](../../../verify/library_checker/data_structure/point_set_range_frequency_dynamic_wavelet.cpp)
