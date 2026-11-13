# rerooting

[索引](../README.md) / [実装](../../../library/tree/rerooting.hpp)

## 機能・前提条件

辺写像とモノイドを指定する全方位木DP。両方向で異なる写像も登録できる。build後にanswers/subanswerを参照する。

## インターフェース

```cpp
#include "library/tree/rerooting.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class F, auto mapping>
struct Rerooting;
Rerooting(int n0);
Rerooting(int n0, vector<S> data0);
void set(int i, S x);
int add_edge(int i, int j, F f);
int add_edge(int i, int j, F f, F g);
void build();
vector<S> answers();
S answer(int v);
vector<S> subanswers(int v);
S subanswer(int id);
```

## 計算量

構築・全回答 O(N)、空間 O(N)。各結合と辺写像のコストを掛ける。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/tree_path_composite_sum_rerooting`](../../../verify/library_checker/tree/tree_path_composite_sum_rerooting.cpp)
- [`unit/recursive_tree`](../../../verify/unit/recursive_tree.cpp)
