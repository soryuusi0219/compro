# splay_tree_lazy

[索引](../README.md) / [実装](../../../library/data_structure/splay_tree_lazy.hpp)

## 機能・前提条件

列に対する挿入・削除・代入・反転と、順方向・逆方向のモノイド集約。位置は現在の列順の0始まり、区間は半開区間です。opは結合的、eは単位元で、非可換演算にも対応します。ノード型・ルート・回転操作は非公開です。

コピー禁止・move可能。move元とsplitの呼出元は空になります。cloneは独立したプールへの複製です。splitで得た列は同じプールを共有しますが、可変ノードを共有せず、互いの更新は独立です。プールは最後の列ハンドルの破棄時にまとめて解放します。

concatは相手を消費して末尾へ連結します。同じプールなら木を接続し、別プールなら値を移してから接続します。どちらも消費元は空になり、先に破棄しても連結先は有効です。sizeはその列の長さ、allocated_nodesは共有プール全体の確保数です。printにはTのostream出力演算子が必要です。eraseしたノードはプール内で再利用し、clearでは個別回収しません。

作用mappingは値と順逆の集約に適合する必要があります。composition(f,g)はf∘g。反転と作用は可換である必要があります。長さ依存の作用にはTに長さを含めてください。

互換名は `lazy_splay_tree<...>`。通常版と同時includeするため、旧 `splay_tree<7引数>` は `LazySplayTree` に変更してください。

公開型は共通のSequenceBaseへの型エイリアスです。既存のメソッド・消費操作・引数は維持しています。独自の前方宣言ではなく、このヘッダーをincludeしてください。通常版と遅延版は集約Policyだけが異なり、回転と列操作を共有します。

## インターフェース

```cpp
#include "library/data_structure/splay_tree_lazy.hpp"
template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using LazySplayTree = /* SequenceBase + action policy + SplayBase */;
LazySplayTree();
explicit LazySplayTree(const std::vector<T>& values);
int size() const;
bool empty() const;
void insert_at(int k, T value);
void erase_at(int k);
void set(int k, T value);
T get(int k);
void flip(int l, int r);
void flip_all();
T prod(int l, int r);
T prodrev(int l, int r);
T prod_all();
T prodrev_all();
void apply(int l, int r, F action);
std::pair<LazySplayTree,LazySplayTree> split(int k);
void concat(LazySplayTree&& other);
LazySplayTree clone();
std::vector<T> to_vector();
void clear();
std::size_t allocated_nodes() const;
void print();
```

## 計算量

列長N、相手の列長M。通常操作・split・同一プールのconcatは償却 O(log(N+1))（concatは合計長に対するlog）。異なるプールのconcatは O(M) と木接続の償却 O(log(N+M+1))。構築・clone・to_vectorは O(N)。size/empty/全体集約/flip_all/clearは O(1)。空間は共有プールの確保数Kに対して O(K)。clear・異プール移送後に到達不能になったノードも、プール終了まで保持します。

## 依存関係

- [internal/reversible_monoid.hpp](internal/reversible_monoid.md)
- [internal/sequence_base.hpp](internal/sequence_base.md)
- [internal/splay_base.hpp](internal/splay_base.md)

## Verify・利用例

- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/dynamic_sequence_range_affine_range_sum_splay_tree](../../../verify/library_checker/data_structure/dynamic_sequence_range_affine_range_sum_splay_tree.cpp)

```cpp
auto [left, right] = sequence.split(k); // sequenceは空
left.concat(std::move(right));         // rightは空
sequence = std::move(left);
```
