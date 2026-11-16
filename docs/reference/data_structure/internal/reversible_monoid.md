# internal/reversible_monoid

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/reversible_monoid.hpp)

## 機能・前提条件

平衡化と所有権から独立した、順逆モノイド集約・反転・遅延作用のPolicy。opは結合的、eは単位元です。順逆の集約を別々に保持し、非可換演算に対応します。作用は集約に適合し、反転と可換である必要があります。

## インターフェース

`ReversibleMonoid<T,op,e,Action=NoTreeAction,Counted=true,Indexed=false,Extra=EmptyTreeState>` を使用します。

Nodeはpp/lp/rp、value、lsum（inorder積）、rsum（逆inorder積）、revを保持します。Countedならcount.size、Indexedならlabel.indexを追加します。Extraは追加の平衡化情報用で、extraとして保持します。不要な状態にはno_unique_addressを用い、通常版にダミーの遅延タグを置きません。

Policyはvalue_type/action_type/Node、identity、pull/push/toggle、size/local_size（Countedの場合）を提供します。priorityを持つExtraにはpriority(node)も提供します。

- `TreeAction<F,mapping,composition,id>`：action_type=F。composition(f,g)はf∘gです。mapping(f,x)は値と順逆集約へ適用します。
- `NoTreeAction`：作用なし。applyは公開されません。
- `apply(p,f)`：非nullなら値・集約・タグへ作用。`toggle(p)`：左右・順逆集約を交換。どちらもnullなら何もしません。
- `pull(p)`：自身の保留タグがpush済みであること。`push(p)`も非nullを要求します。

Countedでのlocal_sizeは常に1です。bit圧縮など複数要素を持つノードには専用Policyを用意します。

## 計算量

各操作 O(1)。Nodeは3本のリンク、Tを3個、反転フラグと選択した追加状態を保持します。モノイド・作用の各演算を O(1) とします。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
