# link_cut

[索引](../README.md) / [実装](../../../library/data_structure/link_cut.hpp)

## 機能・前提条件

動的森のlink/cut、evert、LCA、パス集約。閉路を作るlinkや存在しない辺のcutは不可。

## インターフェース

```cpp
#include "library/data_structure/link_cut.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class T, auto op, auto e>
struct linkcuttree;
struct node;
node(int i,T v);
bool is_root();
void cul();
void rotr();
void rotl();
void toggle();
void push();
void splay();
node* _expose(node* x);
void _cut(node* c);
void _link(node* p, node* c);
void _evert(node *c);
node* _lca(node *u, node *v);
void _revalue(node* c, T x);
vector<int> _get_path(node* x, node* y);
linkcuttree(int n);
void print();
void link(int i, int j);
void cut(int i, int j);
void expose(int i);
void set(int i, T x);
T get(int i);
void evert(int i);
T prod(int i);
T prod(int i, int j);
int lca(int i, int j);
vector<int> get_path(int i,int j);
```

## 計算量

通常操作は償却 O(log N)、パス列挙は出力長を加える。空間 O(N)。

## 詳細な使用方法

頂点は0始まり。`T` は頂点値、`op(T,T)` は結合的演算、`e()` は単位元です。
順方向・逆方向の集約を持ち、非可換な演算でもパスの向きを扱います。

| 操作 | 意味・条件 |
| --- | --- |
| `link(i,j)` | 異なる木の頂点間に辺を追加する。閉路を作らないこと。 |
| `cut(i,j)` | 存在する辺を削除する。 |
| `set(i,x)` / `get(i)` | 頂点値の代入・取得。 |
| `prod(i,j)` | iからjへ向かうパスの積。同一木の頂点を指定する。 |
| `get_path(i,j)` | パスの頂点番号列を返す。戻り値は `vector<int>`。 |
| `evert(i)` | iを表す木の根にする。 |
| `lca(i,j)` | 現在の根に対する最小共通祖先。 |
| `print()` | デバッグ用の木の出力。 |

## 依存関係

標準ライブラリのみ。

## Verify・利用例

- [`library_checker/dynamic_tree_vertex_add_path_sum_link_cut`](../../../verify/library_checker/tree/dynamic_tree_vertex_add_path_sum_link_cut.cpp)
- [`library_checker/dynamic_tree_vertex_set_path_composite_link_cut`](../../../verify/library_checker/tree/dynamic_tree_vertex_set_path_composite_link_cut.cpp)
