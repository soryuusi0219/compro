# modint_traits

[索引](../../README.md) / [実装](../../../../library/math/internal/modint_traits.hpp)

## 機能・前提条件

modint互換型とNTT対応型を検出する型traits。mod()、val()などのインターフェースで判定する。`is_ntt_friendly_v<T>` は符号付き32 bitに収まる静的素数法で、法から1を引いた値が2の20乗で割り切れる場合に真となる。
`internal::has_native_ntt_residue<T>` は、オブジェクト表現が正規化済みの32 bit剰余1語であることを、対応する実装が明示する内部trait。NTTで連続する係数をまとめて読み書きするために使う。

## インターフェース

```cpp
#include "library/math/internal/modint_traits.hpp"
```

- `internal::is_modint_like_v<T>` は `mod()`、`raw(int)`、`val() const` を持つ型を判定する。
- `internal::is_static_modint_like_v<T>` は法をコンパイル時定数として扱える互換型を判定する。
- `internal::is_modint_like_t<T>` と `internal::is_static_modint_like_t<T>` はそれぞれ対応する条件を満たす型に使うSFINAE用の別名。
- `soryuusi::is_ntt_friendly_v<T>` は上記のNTT対応条件を判定する。
- `internal::has_native_ntt_residue<T>` は対応するmodint実装が特殊化する。

## 計算量

コンパイル時評価。実行時の追加時間・空間なし。

## 依存関係

- [internal_math.hpp](internal_math.md)

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
