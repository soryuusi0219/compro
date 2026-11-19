# modint_traits

[索引](../README.md) / [実装](../../../library/math/modint_traits.hpp)

## 機能・前提条件

modint互換型とNTT対応型を検出する型traits。mod()、val()などのインターフェースで判定する。`is_ntt_friendly_v<T>` は符号付き32 bitに収まる静的素数法で、法から1を引いた値が2の20乗で割り切れる場合に真となる。
`internal::has_native_ntt_residue<T>` は、オブジェクト表現が正規化済みの32 bit剰余1語であることを、対応する実装が明示する内部trait。NTTで連続する係数をまとめて読み書きするために使う。

## インターフェース

```cpp
#include "library/math/modint_traits.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template <class T, class = void>
struct is_modint_like : std::false_type;
template <class T>
struct is_modint_like<;
decltype(std::declval<const T&>().val())>> : std::true_type;
template <class T>
template <class T>
using is_modint_like_t = std::enable_if_t<is_modint_like_v<T>>;
template <class T, class = void>
struct is_static_modint_like : std::false_type;
template <class T>
struct is_static_modint_like<;
template <class T>
template <class T>
using is_static_modint_like_t = std::enable_if_t<is_static_modint_like_v<T>>;
template <class T, class = void>
struct is_ntt_friendly : std::false_type;
template <class T>
struct is_ntt_friendly<;
template <class T>
```

## 計算量

コンパイル時評価。実行時の追加時間・空間なし。

## 依存関係

- [internal_math.hpp](internal_math.md)

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
