# splay_explicit

[索引](../README.md) / [実装](../../../library/data_structure/splay_explicit.hpp)

## 機能・前提条件

明示的なキー順に並ぶ値を持つordered set / multiset。キーは固定し、集約・遅延作用は別に持つ値Sへ適用します。Compareは変更されないstrict weak orderingです。Multi=trueなら同値キーを許し、falseなら1キー1要素です。opは結合的でeは単位元。

管理型はarenaだけを持ち、new_set()で独立したSetハンドルを作ります。管理型・Setともコピー不可・move可能。Setは既定構築できず、管理型を破棄・moveしても共有所有するarenaが生存するため有効です。cloneは同一arenaでの独立複製。clearは領域を保持し、eraseしたノードは全Set間で再利用します。

同値キーの挿入は末尾に追加します。Multi=falseのinsertは既存値を変えずfalseを返します。meldは消費元を空にし、同値キーでは連結先の要素を先、消費元を後に置きます。Multi=falseなら連結先の値が優先です。異arenaでもmeld可能で、連結先の比較規則で移送・整列します。

## インターフェース

```cpp
#include "library/data_structure/splay_explicit.hpp"
template<class Key, class S, auto op, auto e,
         bool Multi = true, class Compare = std::less<Key>>
using SplayExplicit = /* arena manager */;
template<class Key, class S, auto op, auto e, class F,
         auto mapping, auto composition, auto id,
         bool Multi = true, class Compare = std::less<Key>>
using LazySplayExplicit = /* lazy arena manager */;
```

管理型：Compareを受け取る構築、Set型、new_set()、new_set(vector<pair<Key,S>>)、allocated_nodes()。初期値はstable sortし、重複不可なら同値キーの最初の値を採用します。

Setの操作：

| 操作 | 意味 |
| --- | --- |
| size(), empty() | 論理要素数・空判定 |
| insert(key,value) | 挿入。成功ならtrue。SがKeyから構築できればinsert(key)も使用可能 |
| lower_bound(key), upper_bound(key) | キー境界の順位。0〜size()の整数を返す |
| count(key), contains(key) | 同値キー数・存在判定 |
| kth(i) | 0始まりi番目のpair<Key,S>を値で返す |
| set_at(i,value) | i番目の値だけを変更 |
| erase(key), erase_all(key) | 最初の同値要素1個／全個を削除し、bool／削除数を返す |
| erase_at(i), erase_range(l,r) | 順位で削除。erase_rangeは削除数を返す |
| prod(l,r), prodrev(l,r) | 順位の半開区間の順／逆集約 |
| prod_all(), prodrev_all() | 全体の順／逆集約 |
| prod_key(low,high) | キー半開区間の集約 |
| apply(l,r,f), apply_key(low,high,f) | 遅延版のみ。順位／キー区間の値へ作用 |
| split(k), split_key(key) | 呼出元を空にして2個のSetを返す。split_keyはlower_boundで分ける |
| meld(std::move(other)) | 集合を統合し、otherを空にする |
| clone(), to_vector(), clear() | 独立複製・inorderのpair配列・空にする |
| allocated_nodes() | 再利用待ちを含む共有arena全体の確保数 |

順位区間は0<=l<=r<=size()。キー区間はCompare順でlowからhigh未満、具体的には !Compare(key,low) && Compare(key,high) を満たす要素です。highがlowより前の呼出しは不可。キーの変更はeraseとinsertで行います。

mappingは値と集約に適合する必要があり、composition(f,g)はf∘g。長さ依存の作用ではSに要素数を含めます。キーには作用しません。Compareを外部から変化させる利用や同じarenaへの並行操作・再入は想定しません。

## 計算量

要素数N。挿入・1要素削除・検索・順位アクセス・区間集約・区間作用・splitは償却 O(log(N+1))。範囲削除は削除数Kを加えます。初期構築は整列込み O(N log(N+1))、clone/to_vectorは O(N)。空の入力との同一arena meldは O(1)。それ以外の同一arena meldはm=min(N,M)に対し保守的に償却 O(m log(N+M+1))、再帰深さ O(log(m+1))。異arenaでは消費元長Mの移送・stable sortに O(M log(M+1)) が追加されます。size/empty/全体集約/clearは O(1)。保持空間はarena全体の確保数Aに対して O(A)、作業領域を加えます。

## 依存関係

- [internal/explicit_base.hpp](internal/explicit_base.md)
- [internal/splay_base.hpp](internal/splay_base.md)

## Verify・利用例

- [unit/ordered_trees](../../../verify/unit/ordered_trees.cpp)
- [library_checker/ordered_set_splay_explicit](../../../verify/library_checker/data_structure/ordered_set_splay_explicit.cpp)
- [library_checker/double_ended_priority_queue_splay_explicit](../../../verify/library_checker/data_structure/double_ended_priority_queue_splay_explicit.cpp)
- [library_checker/range_affine_range_sum_splay_explicit](../../../verify/library_checker/data_structure/range_affine_range_sum_splay_explicit.cpp)

```cpp
soryuusi::SplayExplicit<int, long long, op, e> arena;
auto a = arena.new_set();
auto b = arena.new_set();
a.insert(10, 3);
b.insert(10, 7);
a.meld(std::move(b)); // aは2要素、bは空
assert(a.count(10) == 2 && a.prod_key(10, 11) == 10);
```
