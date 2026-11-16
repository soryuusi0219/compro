# treap

[索引](../README.md) / [実装](../../../library/data_structure/treap.hpp)

## 機能・前提条件

ランダムな優先度で平衡化するimplicit treapによる可変列です。SplayTreeと同じSequenceBaseを使い、位置指定の挿入・削除・代入、反転、順逆の区間集約を提供します。LazyTreapSequenceは区間作用も扱います。

opは結合的、eは単位元。非可換演算にも対応します。mappingは集約に適合し、反転と可換である必要があります。composition(f,g)はf∘gです。長さに依存する作用ではTに長さを含めてください。

コピー禁止・move可能、cloneは独立複製。splitは呼出元を空にし、concatは相手を消費します。同一arena内は木を結合し、異なるarena間は値を移して結合します。サイズ・所有権・区間の規約はsplay版と同じです。優先度の生成は時刻由来のseedを使用します。

## インターフェース

```cpp
#include "library/data_structure/treap.hpp"
template<class T, auto op, auto e>
using TreapSequence = /* SequenceBase + TreapBase */;
template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using LazyTreapSequence = /* SequenceBase + action policy + TreapBase */;
```

両型にデフォルト構築・vector<T>からの構築、size/empty、insert_at/erase_at/set/get、flip/flip_all、prod/prodrev/prod_all/prodrev_all、split/concat、clone/to_vector/clear/allocated_nodes/printがあります。遅延版のみapply(l,r,f)を提供します。添字は0始まり、区間は半開区間です。

詳細な各操作は [splay_tree](splay_tree.md)、[splay_tree_lazy](splay_tree_lazy.md)、共通Backendの契約は [internal/sequence_base](internal/sequence_base.md) を参照してください。

## 計算量

列長N。通常操作・split・同一arenaのconcatは期待 O(log(N+1))、単発最悪 O(N)。構築・clone・to_vectorは O(N)。異arenaのconcatは相手長Mに対し O(M) と結合の期待 O(log(N+M+1))。size/empty/全体集約/flip_all/clearは O(1)。空間は共有arenaの確保数Aに対し O(A)、構築には O(N) 作業空間。優先度に基づく期待計算量であり、最悪対数時間の保証ではありません。

## 依存関係

- [internal/reversible_monoid.hpp](internal/reversible_monoid.md)
- [internal/sequence_base.hpp](internal/sequence_base.md)
- [internal/treap_base.hpp](internal/treap_base.md)

## Verify・利用例

- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)
- [library_checker/range_reverse_range_sum_treap](../../../verify/library_checker/data_structure/range_reverse_range_sum_treap.cpp)
- [library_checker/dynamic_sequence_range_affine_range_sum_treap](../../../verify/library_checker/data_structure/dynamic_sequence_range_affine_range_sum_treap.cpp)
