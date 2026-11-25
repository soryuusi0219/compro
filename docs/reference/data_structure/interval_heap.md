# interval_heap

[索引](../README.md) / [実装](../../../library/data_structure/interval_heap.hpp)

## 機能・前提条件

最小・最大の双方を取り出せるヒープ。Compareに従って順序を定め、同値キーも保持します。get_min/get_max/pop_min/pop_maxは非空時に使います。隣り合う偶数・奇数添字に区間の上端・下端を保持します。構築時はこの対を整え、下降で移動した要素に対して上昇処理を行います。

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

構築 O(N)、木のpush/popは最悪 O(log(N+1))、get_min/get_max O(1)、空間 O(N)。pushにはvectorの確保に伴う再配置が発生することがあります。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/double_ended_priority_queue`](../../../verify/library_checker/data_structure/double_ended_priority_queue.cpp)
- [`unit/data_structure_misc`](../../../verify/unit/data_structure_misc.cpp)
