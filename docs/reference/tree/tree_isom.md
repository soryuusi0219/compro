# tree_isom

[索引](../README.md) / [実装](../../../library/tree/tree_isom.hpp)

## 機能・前提条件

根付き・根なし木の同型判定。子の型IDをソートし共有辞書で同型を分類する。深い木も反復走査で処理し、子の型IDの作業領域を使い回す。

## インターフェース

```cpp
#include "library/tree/tree_isom.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
vector<vector<int>> hashvec(1);
vector<int> get_rooted_hashs(const vector<vector<long long>>& t, int r = 0);
pair<long long, long long> tree_get_centroids(const vector<vector<long long>>& G);
int get_rooted_hash(const vector<vector<long long>>& t, int r = 0);
bool is_rooted_tree_isom(const vector<vector<long long>>& t1, const vector<vector<long long>>& t2, int r1 = 0, int r2 = 0);
bool is_tree_isom(const vector<vector<long long>>& t1, const vector<vector<long long>>& t2);
```

## 計算量

1本の木で O(N log N) 程度のソートに辞書の比較コストを加える。空間 O(N) と共有辞書。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/rooted_tree_isomorphism_classification`](../../../verify/library_checker/tree/rooted_tree_isomorphism_classification.cpp)
- [`unit/tree_algorithms`](../../../verify/unit/tree_algorithms.cpp)
