# math

[索引](../README.md) / [実装](../../../library/math/math.hpp)

## 機能・前提条件

素数判定、原始根、mod冪・逆元、CRT、floor_sumの標準数論API。法は正、逆元は互いに素のときに使う。

## インターフェース

```cpp
#include "library/math/math.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
constexpr bool is_prime(long long n);
constexpr int primitive_root(int m);
constexpr long long pow_mod(long long x, long long n, int m);
template<typename INT1, typename INT2>
constexpr INT2 inv_mod(INT1 x, INT2 m);
template<typename INT = long long>
inline std::pair<INT, INT> crt(const std::vector<INT>& r, const std::vector<INT>& m);
template<typename INT>
constexpr INT floor_sum(INT n, INT m, INT a, INT b);
```

## 計算量

冪・逆元・floor_sumは O(log m) 程度、CRTは法ごとのgcd分の和。素数判定は固定幅Miller–Rabin。空間 O(1)（出入力除く）。

## 依存関係

- [internal_math.hpp](internal_math.md)

## Verify・利用例

- [`library_checker/sum_of_floor_of_linear`](../../../verify/library_checker/number_theory/sum_of_floor_of_linear.cpp)
