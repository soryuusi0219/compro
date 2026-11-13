# li_chao_tree_line

[索引](../README.md) / [実装](../../../library/data_structure/li_chao_tree_line.hpp)

## 機能・前提条件

Li Chao treeで使う直線型 `Line<T>{a,b}` と無限大直線 `INF_Line<T,infinity>()`。`get(x)` は `a*x+b` を返す。

## インターフェース

```cpp
#include "library/data_structure/li_chao_tree_line.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T>
struct Line;
T get(T x);
template<class T, T infinity>
Line<T> INF_Line();
```

## 計算量

直線の生成・評価は O(1) 時間・空間。木の操作は静的版・動的版のreferenceを参照。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
