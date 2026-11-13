# dirichlet_sum

[索引](../README.md) / [実装](../../../library/math/dirichlet_sum.hpp)

## 機能・前提条件

数論関数と商集合上のprefix和を保持しDirichlet畳み込み・除算・冪を行う。型ごとのstatic状態をinit(N,K)で設定する。

## インターフェース

```cpp
#include "library/math/dirichlet_sum.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, int id = -1>
struct Dirichlet_Sum;
static void init(long long n, long long k);
Dirichlet_Sum();
Dirichlet_Sum(vector<T> _al, vector<T> _au, bool is_mul = false);
Dirichlet_Sum(vector<T> _al, T (*f)(long long), bool is_mul = false);
Dirichlet_Sum(T (*fl)(long long), T (*fu)(long long), bool is_mul = false);
Dirichlet_Sum operator+=(const Dirichlet_Sum& other);
Dirichlet_Sum operator+(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator-=(const Dirichlet_Sum& other);
Dirichlet_Sum operator-(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator*=(const T x);
Dirichlet_Sum operator*(const T x) const;
Dirichlet_Sum operator*(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator*=(const Dirichlet_Sum& other);
Dirichlet_Sum operator/(const Dirichlet_Sum& other) const;
Dirichlet_Sum operator/=(const Dirichlet_Sum& other);
Dirichlet_Sum pow(long long k);
inline static Dirichlet_Sum e();
inline static Dirichlet_Sum zeta();
static Dirichlet_Sum zetak(int k);
static Dirichlet_Sum zetashift(int k);
inline T getN();
inline T getN() const;
inline T getNdivi(int i);
inline T getNdivi(int i) const;
inline vector<T> getupper();
inline vector<T> getupper() const;
inline pair<vector<T>,vector<T>> getAll();
inline pair<vector<T>,vector<T>> getAll() const;
template<class T, int id> long long Dirichlet_Sum<T,id>::N;
template<class T, int id> long long Dirichlet_Sum<T,id>::K;
template<class T, int id> long long Dirichlet_Sum<T,id>::L;
template<class T, int id> vector<long long> Dirichlet_Sum<T,id>::eval_points;
template<class T, int id> vector<long long> Dirichlet_Sum<T,id>::primes;
template<class T, int id> vector<long long> Dirichlet_Sum<T,id>::divisors;
```

## 計算量

保持領域 O(K+N/K)。積・除算は商ブロックと約数ループの総数に依存（一般版は上界 O(N log N)）。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/sum_of_totient_function_dirichlet_sum`](../../../verify/library_checker/number_theory/sum_of_totient_function_dirichlet_sum.cpp)
- [`unit/dirichlet_sum`](../../../verify/unit/dirichlet_sum.cpp)
