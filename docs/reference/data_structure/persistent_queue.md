# persistent_queue

[索引](../README.md) / [実装](../../../library/data_structure/persistent_queue.hpp)

## 機能・前提条件

版ごとのpush/popを行う永続キュー。qp が版を表し、新しい版を戻り値から保存する。

## インターフェース

```cpp
#include "library/data_structure/persistent_queue.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct Persistent_Queue;
struct node;
using np=node*;
struct que;
using qp=que*;
struct node;
struct que;
Persistent_Queue();
np get_root();
np get_node(np t, int n);
int size(qp q);
T front(qp q);
bool empty(qp q);
np push_node(const T& x, np t);
qp push(const T& x,qp& t);
qp pop(qp& t);
```

## 計算量

push/pop/祖先取得は O(log Q) を上界とする。Q 更新に O(Q log Q) 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/persistent_queue`](../../../verify/library_checker/data_structure/persistent_queue.cpp)
