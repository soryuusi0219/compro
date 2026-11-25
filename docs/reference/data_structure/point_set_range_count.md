# point_set_range_count

[索引](../README.md) / [実装](../../../library/data_structure/point_set_range_count.hpp)

## 機能・前提条件

長さNの列の一点更新と、半開区間内で値がkに等しい要素数の取得。容量mは利用期間中に現れる異なる値の総数以上にします。hashは値からuint64_tへの関数です。要素未設定のコンストラクタでは未設定位置を数えません。

同じ値への代入は木を更新しません。登録済みの異なる値の総数がmに達した状態で新しい値を代入すると `std::length_error` を投げます。空区間の個数は0です。

コピー禁止、move可能。移動元の木は空になり、再利用できます。`clone()` はノードを共有しない独立複製です。ノードはオブジェクトのプールが所有し、個別に解放しません。 値圧縮情報・現在値の配列・木を一体で複製するので、clone後の更新は独立です。

## インターフェース

```cpp
#include "library/data_structure/point_set_range_count.hpp"
template<class T, auto hash> struct PointSetRangeCount;
PointSetRangeCount(int n, int m);
PointSetRangeCount(const std::vector<T>& values, int m);
void set(int i, const T& value);
int count(int l, int r, const T& value) const;
PointSetRangeCount clone() const;
std::size_t allocated_nodes() const;
template<class T> uint64_t hash_int(T x);
```

## 計算量

値圧縮のハッシュ探索を期待 O(1) として、set/countは O(log N)。確保済みノード数Kに対して空間 O(N+m+K)。削除した値の木も保持するためKは現在の要素数だけでは抑えられず、Q更新に対して保守的には O((N+Q) log N)。cloneは O(N+m+K) 時間・追加空間。

## 依存関係

- [internal/node_pool.hpp](internal/node_pool.md)
- [hash_table.hpp](hash_table.md)
- [internal/hash_table_storage.hpp](internal/hash_table_storage.md)

## Verify・利用例

- [`unit/sparse_structures`](../../../verify/unit/sparse_structures.cpp)
- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/majority_voting](../../../verify/library_checker/data_structure/majority_voting.cpp)
