# treap

[索引](../README.md) / [実装](../../../library/data_structure/treap.hpp)

## 機能・前提条件

0始まりの位置で扱う可変列。挿入・削除・代入、反転、順逆のモノイド集約を提供します。opは結合的、eは単位元です。区間は半開で、非可換演算に対応します。

管理型の宣言ではarenaと平衡化の作業領域だけを確保します。列はnew_sequence()で作成し、型はManager::Sequenceです。列ハンドルは既定構築・コピー不可、move可能。管理型もコピー不可・move可能です。ハンドルがarenaを共有所有するため、管理型を破棄・moveしても既存の列を使用できます。

split(k)は呼出元を空にして先頭k要素と残りを返します。concat(std::move(other))は相手を空にして末尾へ連結。同一arenaではノードを繋ぎ直し、異arenaでは値を移して連結します。結果は連結先のarenaに属します。cloneは同じarenaに独立したノードを作ります。異arenaへの複製は別の管理型のnew_sequence(sequence.to_vector())で行います。clearは個別回収せず、eraseしたノードは同じarena全体で再利用します。

遅延版のmappingは値と集約に適合し、反転と可換である必要があります。composition(f,g)はf∘g。長さ依存の作用には値型に長さも含めてください。

## インターフェース

```cpp
#include "library/data_structure/treap.hpp"
template<class T, auto op, auto e>
using TreapSequence = /* arena manager */;
TreapSequence();
Sequence new_sequence();
Sequence new_sequence(const std::vector<T>& values);
std::size_t allocated_nodes() const;
```

Sequenceの公開操作：size/empty、insert_at/erase_at/set/get、flip/flip_all、prod/prodrev/prod_all/prodrev_all、split/concat、clone/to_vector/clear/allocated_nodes/print。遅延版はLazyTreapSequence<T,op,e,F,mapping,composition,id>で、apply(l,r,f)も使用できます。

get/set/erase_atは0<=i<size()、insert_at/splitは0<=i<=size()。prod/flip/applyは0<=l<=r<=size()。printにはTのostream出力が必要です。moved-fromの列にはsize/empty/clearと再代入・破棄を使用でき、要素を追加する場合は管理型から新しく作ってください。allocated_nodesは共有arena全体の確保数です。

## 計算量

列長N、相手長M。通常操作・split・同一arenaのconcatは期待 O(log(N+M+1))、単発最悪 O(N+M)。管理型構築・空列作成・moveは O(1)。vectorからの構築・clone・to_vectorは O(N)。異arenaのconcatは O(M) と結合の対数時間。size/empty/全体集約/flip_all/clearは O(1)。共有arenaの確保数Aに対して O(A) 空間、作業領域を加えます。move代入で古いarenaの最後の所有者を失う場合は、その解放時間も必要です。

## 依存関係

- [internal/sequence_base.hpp](internal/sequence_base.md)
- [internal/reversible_monoid.hpp](internal/reversible_monoid.md)
- [internal/treap_base.hpp](internal/treap_base.md)

## Verify・利用例

- [unit/ordered_trees](../../../verify/unit/ordered_trees.cpp)
- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/range_reverse_range_sum_treap](../../../verify/library_checker/data_structure/range_reverse_range_sum_treap.cpp)
- [library_checker/dynamic_sequence_range_affine_range_sum_treap](../../../verify/library_checker/data_structure/dynamic_sequence_range_affine_range_sum_treap.cpp)

```cpp
soryuusi::TreapSequence<long long, op, e> arena;
auto left = arena.new_sequence();
auto right = arena.new_sequence();
// left/rightへ要素を追加
left.concat(std::move(right)); // rightは空
auto [a, b] = left.split(left.size()/2);
```
