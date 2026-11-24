# internal/cartesian_tree_base

[索引](../../README.md) / [実装](../../../../library/data_structure/internal/cartesian_tree_base.hpp)

## 機能・前提条件

inorder順の要素からCartesian treeを線形構築する内部関数。整数ID・ポインタのどちらもhandleとして使用でき、出力配列やノードプールを所有しません。比較が同値の場合の規則はhigherが決めます。

## インターフェース

`build_cartesian(n,nil,at,higher,left,right,finish)` を使います。第8引数に親を取得する `parent(handle)` を渡せます。n>=0。

- at(i)はi番目の新しいhandleを返します。
- higher(a,b)は、aをbより上へ置くならtrue。
- left(p,child)/right(p,child)は子を設定します。child=nilも渡されます。必要なら子の親も更新します。
- finish(p)は左右の子の情報が完成した後にちょうど1回呼ばれます。集約更新に利用できます。

`parent` を渡す場合、`at(i)` は親がnilのノードを返し、左右の子を設定するcallbackはその子の親も更新します。現在の右の経路を親リンクでたどります。

返り値は根、空入力ではnilです。再利用ノードを渡す場合、以前の木から切り離し、古い親リンクを残さないでください。

## 計算量

callbackを O(1) とすると要素数Nに対し O(N) 時間。作業空間は標準の構築で O(N)、親リンクを使う場合は O(1) です。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [unit/balanced_trees](../../../../verify/unit/balanced_trees.cpp)
- [unit/pointer_structures](../../../../verify/unit/pointer_structures.cpp)
