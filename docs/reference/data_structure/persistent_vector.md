# persistent_vector

[索引](../README.md) / [実装](../../../library/data_structure/persistent_vector.hpp)

## 機能・前提条件

非負整数添字の永続配列。未設定値は `T{}`。8分木を添字の上位からたどり、更新経路だけを複製します。葉には連続した添字の値を格納します。値が自明にコピーできる場合、葉に収まる範囲で複数の値を詰めます。

`Root` は不透明な借用版ハンドルです。コピーは O(1) でノードを共有しますが、公開済みノードは変更しません。更新は新しい版を返し、検索は入力版を変更せず、ノードも確保しません。管理オブジェクトはコピー禁止・move可能で、版は移動先で引き続き使用できます。管理領域の破棄・move代入による置換後は、その領域の版を使用できません。別の管理領域の版を混ぜる操作は不可で、デバッグ時には所有元を検査します。移動元の管理オブジェクトは破棄または再代入してください。 `build` は新しい未公開木だけを破壊的に構築し、過去の版には触れません。旧des_set/mutable_getとノード型の公開は廃止しました。

## インターフェース

```cpp
#include "library/data_structure/persistent_vector.hpp"
template<class T> class Persistent_Vector;
using Root = /* opaque handle */;
Root empty_root() const;
Root get_root() const; // empty_rootの互換名
Root build(const std::vector<T>& values);
Root set(Root root, int i, const T& value);
T get(Root root, int i) const;
Root set(int i, const T& value, Root root); // 旧引数順
T get(int i, Root root) const; // 旧引数順
std::size_t allocated_nodes() const;
```

## 計算量

葉の要素数をBとし、操作する版の最大設定添字と対象添字の大きい方をIとします。木の高さを含む経路長Dは O(1 + log₈(1 + I/B))。getは最悪 O(D)、setは最悪 O(D) 時間・追加ノード数です。Tのコピー・構築は定数時間とします。長さNのbuildは O(N) 時間・O(1 + N/B)ノード。各ノードは8本の子ポインタまたは葉の値を格納します。全版のメモリは管理オブジェクトが一括所有します。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)
- [math/internal/internal_bit.hpp](../math/internal/internal_bit.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)

```cpp
soryuusi::Persistent_Vector<int> array;
auto r0 = array.empty_root();
auto r1 = array.set(r0, 3, 10);
auto r2 = array.set(r0, 3, 20); // r0から別に分岐
assert(array.get(r1, 3) == 10);
```
