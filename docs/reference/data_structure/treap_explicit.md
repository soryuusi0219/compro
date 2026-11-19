# treap_explicit

[索引](../README.md) / [実装](../../../library/data_structure/treap_explicit.hpp)

## 機能・前提条件

明示的なキー順に並ぶ値を持つordered set / multiset。集約は別に持つ値Sへ適用します。Monotone版はキーへの遅延作用、LazyMonotone版はキーと値への遅延作用を提供します。Compareは変更されないstrict weak orderingです。Multi=trueなら同値キーを許し、falseなら1キー1要素です。opは結合的でeは単位元。

管理型はarenaだけを持ち、new_set()で独立したSetハンドルを作ります。管理型・Setともコピー不可・move可能。Setは既定構築できず、管理型を破棄・moveしても共有所有するarenaが生存するため有効です。cloneは同一arenaでの独立複製。clearは領域を保持し、eraseしたノードは全Set間で再利用します。

同値キーの挿入は末尾に追加します。Multi=falseのinsertは既存値を変えずfalseを返します。meldは消費元を空にし、同値キーでは連結先の要素を先、消費元を後に置きます。Multi=falseなら連結先の値が優先です。異arenaでもmeld可能で、連結先の比較規則で移送・整列します。

## インターフェース

```cpp
#include "library/data_structure/treap_explicit.hpp"
template<class Key, class S, auto op, auto e,
         bool Multi = true, class Compare = std::less<Key>>
using TreapExplicit = /* arena manager */;
template<class Key, class S, auto op, auto e, class F,
         auto mapping, auto composition, auto id,
         bool Multi = true, class Compare = std::less<Key>>
using LazyTreapExplicit = /* lazy arena manager */;
template<class Key, class S, auto op, auto e, class F,
         auto key_mapping, auto composition, auto id,
         bool Multi = true, class Compare = std::less<Key>>
using MonotoneTreapExplicit = /* arena manager with lazy key actions */;
template<class Key, class S, auto op, auto e,
         class KF, auto key_mapping, auto key_composition, auto key_id,
         class F, auto mapping, auto composition, auto id,
         bool Multi = true, class Compare = std::less<Key>>
using LazyMonotoneTreapExplicit = /* arena manager with lazy key and value actions */;
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
| ref_at(i), index_of(ref) | 要素参照を取得／この集合内の現在順位を返す。見つからなければ-1 |
| set_at(i,value) | i番目の値だけを変更 |
| erase(key), erase_all(key) | 最初の同値要素1個／全個を削除し、bool／削除数を返す |
| erase_at(i), erase_range(l,r) | 順位で削除。erase_rangeは削除数を返す |
| prod(l,r), prodrev(l,r) | 順位の半開区間の順／逆集約 |
| prod_all(), prodrev_all() | 全体の順／逆集約 |
| prod_key(low,high) | キー半開区間の集約 |
| apply(l,r,f), apply_key(low,high,f) | Lazy版・LazyMonotone版で順位／キー区間の値へ作用 |
| apply_keys(l,r,f,decreasing=false) | Monotone版・LazyMonotone版で順位区間にキー作用 |
| apply_keys_by_key(low,high,f,decreasing=false) | Monotone版・LazyMonotone版で更新前のキー区間にキー作用 |
| split(k), split_key(key) | 呼出元を空にして2個のSetを返す。split_keyはlower_boundで分ける |
| meld(std::move(other)) | 集合を統合し、otherを空にする |
| clone(), to_vector(), clear() | 独立複製・inorderのpair配列・空にする |
| allocated_nodes() | 再利用待ちを含む共有arena全体の確保数 |

順位区間は0<=l<=r<=size()。キー区間はCompare順でlowからhigh未満、具体的には !Compare(key,low) && Compare(key,high) を満たす要素です。highがlowより前の呼出しは不可。固定キーの型でキーを差し替える場合はeraseとinsertを使います。

mappingは値と集約に適合する必要があり、composition(f,g)はf∘g。長さ依存の作用ではSに要素数を含めます。applyとapply_keyは値Sへ作用します。Compareを外部から変化させる利用や同じarenaへの並行操作・再入は想定しません。

キー作用では `key_mapping(f,key)` が新しいキーを返します。キー作用の合成も f∘g の順で、
恒等作用を定義し、作用型は `!=` で比較できる必要があります。作用は選択範囲内でCompare順を
保つ単調増加、または `decreasing=true` でCompare順を逆転する単調減少とします。
Multi=trueでは狭義でない単調性も許し、減少方向は同値要素も含めて範囲全体を反転します。
Multi=falseでは範囲内で異なるキーが同値にならない、狭義単調な作用が前提です。
値Sは元の要素に付いたまま移動し、順・逆集約は新しい順序に従います。

LazyMonotone版のテンプレート引数はキー作用 `KF,key_mapping,key_composition,key_id`、
値作用 `F,mapping,composition,id` の順です。作用はそれぞれ独立に合成します。
キーの減少方向の更新で要素を反転すると、保留中の値作用も対象の要素に引き継がれます。
`apply_key(low,high,f)` で値を更新する対象は、呼出し時点のキーで選びます。

更新後も区間の前後と整列していればその位置へ接続します。キーが境界を越えた場合や、
Multi=falseで区間外のキーと衝突した場合は、更新しなかった集合へ更新区間をmeldします。
この経路では同値キーで区間外の要素が先になり、Multi=falseでは区間外の要素を残します。
Multi=trueで境界が同値でも整列している場合は、そのまま前・更新区間・後の順に並びます。

`ref_at(i)` は0始まりの要素を指すコピー可能な `NodeRef` を返します。`index_of(ref)` は現在の順位を返し、
その列・集合に要素がなければ-1を返します。反転・遅延作用・split・moveや、異なるarena間の移送でも
残った要素を追跡できます。erase・clear・衝突による除去で参照先を失い、cloneは独立した要素を作ります。
参照単体はarenaの寿命を延ばしません。ノードが再利用されても古い参照は復活しません。

## 計算量

要素数N。挿入・1要素削除・検索・順位アクセス・区間集約・区間作用・splitは期待 O(log(N+1))。範囲削除は削除数Kを加えます。初期構築は整列込み O(N log(N+1))、clone/to_vectorは O(N)。空の入力との同一arena meldは O(1)。それ以外の同一arena meldでは、小さい方をm、大きい方をnとします。重複可なら期待 O(m log(1+n/m))、再帰深さは期待 O(log(n+m+1))。重複不可なら保守的に期待 O(m log(n+m+1))、再帰深さ O(log(m+1))。異arenaでは消費元長Mの移送・stable sortに O(M log(M+1)) が追加されます。size/empty/全体集約/clearは O(1)。保持空間はarena全体の確保数Aに対して O(A)、作業領域を加えます。

キー作用は更新後も境界と整列していれば期待 O(log(N+1))。
meldが必要なら、更新要素数Kに対して期待 O((1+min(K,N-K)) log(N+1))。
参照の取得と所属集合での順位取得は期待 O(log(N+1))。同一arenaの別の集合や
取り除かれた木を指す場合、順位取得は元の木の高さに比例します。

## 依存関係

- [internal/explicit_base.hpp](internal/explicit_base.md)
- [internal/treap_base.hpp](internal/treap_base.md)

## Verify・利用例

- [unit/ordered_trees](../../../verify/unit/ordered_trees.cpp)
- [library_checker/ordered_set_treap_explicit](../../../verify/library_checker/data_structure/ordered_set_treap_explicit.cpp)
- [library_checker/double_ended_priority_queue_treap_explicit](../../../verify/library_checker/data_structure/double_ended_priority_queue_treap_explicit.cpp)
- [library_checker/range_affine_range_sum_treap_explicit](../../../verify/library_checker/data_structure/range_affine_range_sum_treap_explicit.cpp)

```cpp
soryuusi::TreapExplicit<int, long long, op, e> arena;
auto a = arena.new_set();
auto b = arena.new_set();
a.insert(10, 3);
b.insert(10, 7);
a.meld(std::move(b)); // aは2要素、bは空
assert(a.count(10) == 2 && a.prod_key(10, 11) == 10);
```
