# splay_tree

[索引](../README.md) / [実装](../../../library/data_structure/splay_tree.hpp)

## 機能・前提条件

挿入・削除・反転と順逆の区間集約を扱う列splay木。添字は現在の列順。

## インターフェース

```cpp
#include "library/data_structure/splay_tree.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto op, auto e>
struct splay_tree;
struct node;
node(int i, T v);
void cul();
void rotr();
void rotl();
void toggle();
void push();
void splay();
using pnode = unique_ptr<node>;
node* most_left(node* c);
node* most_right(node* c);
node* kth_node(node* c, int k);
node* next(node* c);
node* prev(node* c);
pair<node*, node*> split_right(node* c);
pair<node*, node*> split_left(node* c);
node* merge(node* c, node* d);
node* make_node(T v);
void delete_node(node* c);
node* insert_right(node* c, T v);
node* insert_left(node* c, T v);
node* erase(node* c);
int size();
void insert_at(int k, T v);
void erase_at(int k);
T get(int k);
void flip(int l, int r);
void flip_all();
T prod(int l, int r);
T prodrev(int l, int r);
T prod_all();
T prodrev_all();
splay_tree();
splay_tree(vector<T>& v);
void print();
```

## 計算量

通常操作は償却 O(log N)、構築 O(N)、空間 O(N)。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/range_reverse_range_sum_splay_tree`](../../../verify/library_checker/data_structure/range_reverse_range_sum_splay_tree.cpp)
