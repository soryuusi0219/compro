# interval_heap

[索引](../README.md) / [実装](../../../library/data_structure/interval_heap.hpp)

## 機能・前提条件

最小・最大の双方を取り出せるヒープ。Compare に従って順序を定める。空の集合から取り出さない。

## インターフェース

```cpp
#include "library/data_structure/interval_heap.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<typename T, typename Compare = less<T>>
struct Interval_Heap;
Interval_Heap();
Interval_Heap(const vector<T> &v, Compare comp_ = Compare());
template<class Iter> Interval_Heap(Iter first, Iter last, Compare comp_ = Compare());
bool empty();
int size();
T get_max();
T get_min();
void make_heap();
void pop_min();
void pop_max();
void push(T x);
inline int parent(int k);
int down(int k);
int up(int k, int root = 1);
```

## 計算量

構築 O(N)、push/pop O(log N)、get_min/get_max O(1)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/double_ended_priority_queue`](../../../verify/library_checker/data_structure/double_ended_priority_queue.cpp)
