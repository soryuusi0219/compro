# treap_sortable

[索引](../README.md) / [実装](../../../library/data_structure/treap_sortable.hpp)

## 機能・前提条件

キーと値のペアの可変列です。キーで区間を昇順・降順にソートし、順序に依存する区間集約を求められます。
ソート済み区間の要素はExplicit木に置き、区間の並びは要素数で重み付けしたSequenceの木に置きます。
`Compare` は固定のstrict weak orderingです。同値キーもすべて保持し、ソート時の同値要素の相対順は未規定です。
ソート方向は `Compare` を基準にします。値の演算 `op` は結合的で、`e()` は単位元です。

管理型は `new_sequence()` で列ハンドルを作ります。管理型・列ハンドルはmove可能でコピー不可です。
列がarenaを共有所有するため、管理型の破棄後も列を使用できます。

## インターフェース

```cpp
#include "library/data_structure/treap_sortable.hpp"

template<class Key, class S, auto op, auto e, class Compare = std::less<Key>>
using TreapSortable = /* arena manager */;
template<class Key, class S, auto op, auto e, class F, auto mapping, auto composition, auto id,
         class Compare = std::less<Key>>
using LazyTreapSortable = /* lazy value actions */;
template<class Key, class S, auto op, auto e, class KF, auto key_mapping, auto key_composition, auto key_id,
         class Compare = std::less<Key>>
using MonotoneTreapSortable = /* lazy monotone key actions */;
template<class Key, class S, auto op, auto e,
         class KF, auto key_mapping, auto key_composition, auto key_id,
         class F, auto mapping, auto composition, auto id, class Compare = std::less<Key>>
using LazyMonotoneTreapSortable = /* both actions */;
```

管理型は `Compare` を受け取るコンストラクタ、`Sequence`、`Entry = std::pair<Key,S>`、`NodeRef`、
`new_sequence()`、`new_sequence(const std::vector<Entry>&)`、`allocated_nodes()` を提供します。
vectorからの構築では入力順を保ちます。添字は0始まり、区間は半開です。

| Sequenceの操作 | 意味 |
| --- | --- |
| `size()`, `empty()` | 要素数、空判定 |
| `get(i)`, `kth(i)` | i番目のキー・値をEntryで返す |
| `set(i,key,value)`, `set(i,entry)` | i番目のキーと値を変更 |
| `set(i,value)`, `set_at(i,value)` | i番目の値を変更 |
| `insert_at(i,key,value)`, `insert_at(i,entry)` | i番目の直前へ挿入。末尾も指定可能 |
| `erase_at(i)`, `erase_range(l,r)` | 削除。区間版は削除数を返す |
| `sort(l,r,decreasing=false)` | キーで区間をソート。`sort_inc(l,r)` と `sort_dec(l,r)` でも指定可能 |
| `flip(l,r)`, `flip_all()` | 区間、または列全体を反転 |
| `prod(l,r)`, `prodrev(l,r)` | 区間の順・逆集約 |
| `prod_all()`, `prodrev_all()` | 列全体の順・逆集約 |
| `apply(l,r,f)` | Lazy版・LazyMonotone版。区間の値Sに作用 |
| `apply_keys(l,r,f,decreasing=false)` | Monotone版・LazyMonotone版。区間のキーに作用 |
| `split(k)`, `concat(std::move(other))` | 先頭k要素と残りへ分割、または相手を末尾へ連結。呼出元、または連結相手を空にする |
| `ref_at(i)`, `index_of(ref)` | 要素を指す不透明な参照を取得、またはこの列での現在順位を返す。列に含まれなければ-1 |
| `clone()`, `to_vector()`, `clear()` | 独立複製、列順のEntry配列、空にする |
| `allocated_nodes()` | 共有arenaの内側・外側の確保ノード数の合計 |

値作用の `mapping(f,x)` は要素と集約の双方に使います。長さが必要な作用ではSに長さを含めます。
キー作用の `key_mapping(f,key)` は `Compare` に関して単調増加、`decreasing=true` の場合は
単調減少である必要があります。どちらも狭義である必要はありません。
各作用は `composition(f,g)` で f∘g の順に合成し、恒等作用と比較できる型を使います。
キー作用は現在の列順を保ちます。更新後のキーで並べるには `sort` を呼びます。

参照はソート、反転、点更新、分割、別arenaへの連結でも同じ要素を追います。
削除・clear後は無効になり、cloneでできた別の要素は指しません。参照はarenaの寿命を延ばしません。

## 計算量

Nを列長とし、演算・作用・比較を定数時間とします。点操作、区間集約・作用・反転、split、
同一arenaのconcat、所属する参照の順位アクセスは期待 O(log(N+1))。concatでは両方の列長の和をNとします。
同一arena内の別の列の参照を調べる場合は、その要素を持つ内外の木の高さに比例します。
範囲削除は削除数Kに対して O(K) が加わります。

ソート後は対象が1区間になります。境界分割後にB区間を含むとき、
外側の分割・結合と区間の走査に期待 O(log(N+1) + B)。
内側の逐次meldでは、各回に併合する2区間の短い方をm、長い方をnとして期待 O(log(N+1) + m log(1+n/m)) が加わります。
キー範囲が交差しない2区間は期待 O(log(N+1)) で連結します。

構築・clone・to_vectorは O(N)。異arenaのconcatは相手長Mの移送に O(M) が加わります。
size・全体集約・flip_all・clearは O(1)。空間はarenaに確保済みのノード数に比例します。
削除とソートで不要になったノードは再利用し、clearでは一括で切り離します。

## 依存関係

- [internal/sortable_base.hpp](internal/sortable_base.md)
- [internal/treap_base.hpp](internal/treap_base.md)

## Verify・利用例

- [library_checker/point_set_range_sort_range_composite_treap](../../../verify/library_checker/data_structure/point_set_range_sort_range_composite_treap.cpp)
- [unit/sortable_trees](../../../verify/unit/sortable_trees.cpp)

```cpp
soryuusi::TreapSortable<int, S, op, e> arena;
auto seq = arena.new_sequence({{5, a}, {1, b}, {3, c}});
auto ref = seq.ref_at(0);
seq.sort_inc(0, 3); // キー順は 1, 3, 5
assert(seq.index_of(ref) == 2);
S combined = seq.prod(0, 3);
```
