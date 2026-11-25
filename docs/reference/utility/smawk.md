# smawk

[索引](../README.md) / [実装](../../../library/utility/smawk.hpp)

## 機能・前提条件

全単調行列の要素を `f(row,col)` で評価し、各行の `{最良列, 値}` を返す。行・列は0始まり。
`Compare` の優先順で選び、同値なら最も左の列を返す。任意の行・列を取り出した部分行列で、最良列は下の行へ向かって広義単調増加であること。行数・列数は非負で、行があれば列は1つ以上必要。

帯状の有効範囲がある場合は、範囲外を含めて全単調になるように `f` を定義する。例えば凸列のmin-plus畳み込みでは、凸列の両端を十分に急な一定傾斜で延長できる。傾斜は元の凸性を保ち、範囲外の候補が選ばれず、値が係数型に収まるように選ぶ。[min-plus畳み込みのverify](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_smawk_utility.cpp)

## インターフェース

```cpp
#include "library/utility/smawk.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, class Compare = less<T>>
vector<pair<int,T>> SMAWK(int h, int w, const auto f, const Compare &comp = Compare());
```

## 計算量

$H$ 行 $W$ 列に対して $O(H+W)$ 回の要素評価・比較。元の列番号は連番として扱い、各段の候補を1つの作業配列に格納する。出力を含め $O(H)$ 空間。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/min_plus_convolution_convex_arbitrary_smawk_utility`](../../../verify/library_checker/convolution/min_plus_convolution_convex_arbitrary_smawk_utility.cpp)
- [`unit/monge`](../../../verify/unit/monge.cpp)
