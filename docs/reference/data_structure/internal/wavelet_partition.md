# wavelet_partition

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/wavelet_partition.hpp)

## 機能・前提条件

Wavelet Matrixの一階層について、キーの指定ビットをrank用のビット列に保存し、ビットが0の要素、1の要素の順に安定分割する。キーに対応する重みなどは同じ順に並べる。

## インターフェース

```cpp
template<class T> class WaveletPartition;
explicit WaveletPartition(const std::vector<T>& keys);
int split(std::vector<T>& keys, int bit, StaticBitVector& rank, auto take);
```

構築時の `keys` と各階層の `keys` は同じ要素を持つ。 `split` は分割前の順序でrankを構築し、0ビットの個数を返す。両方のビットを含む階層では、分割後の順序で分割前の添字を `take(i)` に渡し、最後に作業バッファとキーを交換する。ビットが一種類の階層では並びが変わらないため、`take` は呼ばれない。

## 計算量

初期化と一階層の処理は最悪 `O(N+1)`、作業領域は `O(N)`。64ビット以下の整数型のキーは構築時にビット単位のORとANDを求める。各キーが同じビットを持つ階層ではrank用のワードだけを設定し、`O(N/64+1)` で処理する。

## 依存関係

- [internal_bit.hpp](../../math/internal/internal_bit.md)
- [static_bit_vector.hpp](../static_bit_vector.md)

## Verify・利用例

- [`unit/static_rank_and_order_statistics`](../../../../verify/unit/static_rank_and_order_statistics.cpp)
- [`library_checker/point_add_rectangle_sum_wavelet_bit`](../../../../verify/library_checker/data_structure/point_add_rectangle_sum_wavelet_bit.cpp)
