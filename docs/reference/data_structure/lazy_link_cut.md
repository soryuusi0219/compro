# lazy_link_cut

[索引](../README.md) / [実装](../../../library/data_structure/lazy_link_cut.hpp)

## 機能・前提条件

頂点 `[0,n)` の動的森。辺の追加・削除、根の変更、LCAと方向付きパス集約を扱います。Tは頂点値、opは結合的演算、eは単位元です。非可換演算にも対応します。linkは異なる木の頂点間、cutは存在する辺に限ります。パス・LCAは同一木の頂点を指定してください。

コピー禁止・move可能。ノード配列は非公開で、構築後はサイズを変えません。cloneは全リンクを新配列内へ付け替える独立複製です。move元は頂点数0になります。複数の木は1つの管理オブジェクト内に置き、別オブジェクトのノードを直接接続しません。

connected/sameは連結判定、find_rootは現在の根です。evertで根が変わるほか、2頂点prod・apply・get_pathは第2引数側へevertします。prod(i)はiから現在の根へ続くパスの集約を返します。get_path(i,j)はiからjの順で頂点を列挙します。閉路link・非辺cutはデバッグ時にassertで検出します。

mappingは値と順逆の集約に適合する必要があり、composition(f,g)はf∘gです。長さ依存の作用ではTに長さも含めます。apply(i,f)は現在の根からiへのパス、apply(i,j,f)は2頂点間のパスへ作用します。

公開型はLinkCutBaseへの型エイリアスです。既存の呼出し形式を維持し、通常版と遅延版はPolicyを切り替えて同じ補助木・LCT操作を使用します。

## インターフェース

```cpp
#include "library/data_structure/lazy_link_cut.hpp"
template<class T, auto op, auto e, class F, auto mapping, auto composition, auto id>
using lazy_linkcuttree = /* LinkCutBase + action policy */;
lazy_linkcuttree(int n);
lazy_linkcuttree(int n, T initial_value);
int size() const;
void link(int i, int j);
void cut(int i, int j);
bool connected(int i, int j);
bool same(int i, int j);
int find_root(int i);
void set(int i, T value);
T get(int i);
void expose(int i);
void evert(int i);
T prod(int i);
T prod(int i, int j);
int lca(int i, int j);
std::vector<int> get_path(int i, int j);
void apply(int i, F action);
void apply(int i, int j, F action);
lazy_linkcuttree clone() const;
void print();
```

## 計算量

頂点数N。構築・cloneは O(N) 時間・空間。link/cut/evert/lca/find_root/connected/一点操作/パス操作は償却 O(log(N+1))。get_pathはさらに出力長分を要します。sizeは O(1)。内部splayの祖先タグ伝播には管理オブジェクトごとの再利用作業配列を使い、最大 O(N) 空間を要します。

## 依存関係

- [internal/link_cut_base.hpp](internal/link_cut_base.md)
- [internal/reversible_monoid.hpp](internal/reversible_monoid.md)

## Verify・利用例

- [unit/balanced_trees](../../../verify/unit/balanced_trees.cpp)

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/dynamic_tree_vertex_add_path_sum_lazy_link_cut](../../../verify/library_checker/tree/dynamic_tree_vertex_add_path_sum_lazy_link_cut.cpp)

- [library_checker/dynamic_tree_vertex_set_path_composite_lazy_link_cut](../../../verify/library_checker/tree/dynamic_tree_vertex_set_path_composite_lazy_link_cut.cpp)
