# persistent_dsu_with_data

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dsu_with_data.hpp)

## 機能・前提条件

union by sizeを使う永続Union-Find。頂点は `[0,n)`。検索では経路圧縮を行いません。`empty_root()` は全頂点が独立した初期版で、要素数0の意味ではありません。initは管理オブジェクトあたり1回だけ呼べます。

`Root` は不透明な借用版ハンドルです。コピーは O(1) でノードを共有しますが、公開済みノードは変更しません。更新は新しい版を返し、検索は入力版を変更せず、ノードも確保しません。管理オブジェクトはコピー禁止・move可能で、版は移動先で引き続き使用できます。管理領域の破棄・move代入による置換後は、その領域の版を使用できません。別の管理領域の版を混ぜる操作は不可で、デバッグ時には所有元を検査します。移動元の管理オブジェクトは破棄または再代入してください。 成分値のopは結合的・可換である必要があります。setは頂点単独ではなく、その頂点の成分の値を置き換えます。 leader/same/size/get/setにはrootを最後に渡す従来の引数順も残します。名前空間直下のnpは廃止しました。

## インターフェース

```cpp
#include "library/data_structure/persistent_dsu_with_data.hpp"
template<class S, auto op> class PersistentDSUWithData;
using Root = /* opaque handle */;
using np = Root;
PersistentDSUWithData();
PersistentDSUWithData(int n, S initial_value);
Root init(int n, S initial_value);
Root empty_root() const;
int leader(Root root, int i) const;
bool same(Root root, int i, int j) const;
int size(Root root, int i) const;
Root merge(Root root, int i, int j);
std::pair<int,Root> merge(int i, int j, Root root); // 正しい新代表も返す
std::size_t allocated_nodes() const;
S get(Root root, int i) const;
Root set(Root root, int i, S value);
// 別名: Persistent_dsu_with_data<S,op>
```

## 計算量

頂点数N。検索・併合は最悪 O(log N · log₂₀(N+1)) 時間。mergeは O(log₂₀(N+1))ノードを追加します。初期化は O(N log₂₀(N+1)) 時間・O(N+1)ノード。成分setは O(log N · log₂₀(N+1)) 時間、O(log₂₀(N+1))ノード追加。版のコピーと初期版の取得は O(1)。

## 依存関係

- [persistent_vector.hpp](persistent_vector.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/persistent_unionfind_with_data](../../../verify/library_checker/data_structure/persistent_unionfind_with_data.cpp)
