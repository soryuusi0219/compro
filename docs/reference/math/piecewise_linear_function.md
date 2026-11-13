# piecewise_linear_function

[索引](../README.md) / [実装](../../../library/math/piecewise_linear_function.hpp)

## 機能・前提条件

長さ・傾き・jumpの列で区分線形関数を表す。平行移動、線形結合、評価、包絡などを扱う。

## インターフェース

```cpp
#include "library/math/piecewise_linear_function.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <class T>
struct PiecewiseLinearFunction;
struct Segment;
PiecewiseLinearFunction(T start_x = 0, T init_y = 0);
inline T get_len() const;
inline T get_left() const;
inline T get_right() const;
inline PiecewiseLinearFunction& shift_x(T dx);
inline PiecewiseLinearFunction& shift_y(T dy);
inline PiecewiseLinearFunction& shift(T dx, T dy);
inline size_t size() const;
PiecewiseLinearFunction& add_linear(T a, T b);
T get_min_with_linear(T a, bool has_jmp = false) const;
T eval(T x) const;
vector<T> to_vect(bool include_right = false);
void push_back(T length, T slope, T jump = 0);
void push_front(T length, T slope, T jump = 0);
void pop_front(T len);
void pop_back(T len);
static PiecewiseLinearFunction linear_combine(PiecewiseLinearFunction F, PiecewiseLinearFunction G, T a = 1, T b = 1);
friend PiecewiseLinearFunction operator+(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g);
friend PiecewiseLinearFunction operator-(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g);
friend PiecewiseLinearFunction& operator+=(PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g);
friend PiecewiseLinearFunction& operator-=(PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g);
friend PiecewiseLinearFunction& operator*=(PiecewiseLinearFunction& f, T c);
friend PiecewiseLinearFunction operator*(PiecewiseLinearFunction f, T c);
PiecewiseLinearFunction operator-() const;
void merge_max_straight(T& bk, T len, T f_y0, T g_y0, T f_slope, T g_slope);
static PiecewiseLinearFunction max_of_scalar_multiple(PiecewiseLinearFunction F, PiecewiseLinearFunction G, T a = 1, T b = 1);
static PiecewiseLinearFunction pointwise_max(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g);
static PiecewiseLinearFunction pointwise_min(const PiecewiseLinearFunction& f, const PiecewiseLinearFunction& g);
static PiecewiseLinearFunction max_plus_convolution(const PiecewiseLinearFunction& F, const PiecewiseLinearFunction& G);
```

## 計算量

区間数 S のeval/最小値 O(S)、線形結合・包絡は入力区間数の和に比例する走査。空間は出力区間数に比例。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`unit/piecewise_linear_function`](../../../verify/unit/piecewise_linear_function.cpp)
