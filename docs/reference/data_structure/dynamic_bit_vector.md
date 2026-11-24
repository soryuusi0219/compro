# dynamic_bit_vector

[索引](../README.md) / [実装](../../../library/data_structure/dynamic_bit_vector.hpp)

## 機能・前提条件

挿入・削除・一点更新・rankを扱う可変bit列です。64bit整数にbitを詰めたブロックを、重み付きimplicit treapに載せます。DynamicWaveletの内部bit列から独立させたライブラリです。

位置は0始まり、rank(k)は半開区間 `[0,k)` の1の個数です。空列でもsize/empty/rank(0)を使用できます。構築時は48bitずつ詰め、最後のブロックは実bit数を保持します。挿入で64bitになったブロックは32bitずつに分けます。末尾への追加で分割する場合は前48bit・後16bitにします。削除で8bitに縮んだブロックは、直後のブロックと合わせて32bit以下なら併合します。空になったブロックと、併合で不要になったブロックは再利用します。

コピー禁止・move可能で、移動元は空になり再利用できます。cloneは独立した複製です。ノードの所有権はプールが持ち、削除したノードは再利用します。ノードやルートは公開しません。

## インターフェース

```cpp
#include "library/data_structure/dynamic_bit_vector.hpp"
class DynamicBitVector;
DynamicBitVector();
explicit DynamicBitVector(const std::vector<bool>& values);
void build(const std::vector<bool>& values);
int size() const;
bool empty() const;
bool get(int i) const;
void set(int i, bool value);
void insert(int i, bool value);
void erase(int i);
int rank(int k) const;
int rank(int l, int r) const;
std::vector<bool> to_vector() const;
DynamicBitVector clone() const;
std::size_t allocated_nodes() const;
```

get/set/eraseは `0 <= i < size()`、insertは `0 <= i <= size()`。
buildは現在の内容と確保済み領域を置き換えます。allocated_nodesは再利用待ちを含む確保数で、現在のブロック数とは異なります。constな読み取りでノードは追加しません。`rank(0)`、`rank(size())` と全0・全1の列のrankは集計値から直接返します。区間rankは両端の探索経路を分岐点まで共有します。

## 計算量

現在のbit数N、到達可能なブロック数K、プールの確保数A。1ブロックは1〜63bitでK<=Nです。
get/set/insert/erase/rankは期待 O(log(K+1))、最悪 O(K)。buildとto_vectorは O(N)。cloneは元のブロックをワード単位で読み直すので、O(K + ceil(N/48)) 時間でコンパクトな独立複製を作ります。size/empty/allocated_nodesは O(1)。空間は O(A) で、構築・走査はノードおよび出力以外に O(1) の作業領域を使います。buildで置換する古い領域の破棄時間も必要です。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)
- [internal/treap_base.hpp](internal/treap_base.md)
- [internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)：ブロック境界、rank、構築・再構築、move/clone、削除後の再利用を配列と比較。
- [library_checker/point_set_range_frequency_dynamic_wavelet](../../../verify/library_checker/data_structure/point_set_range_frequency_dynamic_wavelet.cpp)：Wavelet内部で使用。

```cpp
soryuusi::DynamicBitVector bits(std::vector<bool>{true, false, true});
bits.insert(1, true);
assert(bits.size() == 4 && bits.rank(3) == 2);
```
