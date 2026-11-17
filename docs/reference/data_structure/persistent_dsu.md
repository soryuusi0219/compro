# persistent_dsu

[索引](../README.md) / [実装](../../../library/data_structure/persistent_dsu.hpp)

## 機能・前提条件

union by sizeを使う永続Union-Find。頂点は `[0,n)`。検索では経路圧縮を行いません。`empty_root()` は全頂点が独立した初期版で、要素数0の意味ではありません。initは管理オブジェクトあたり1回だけ呼べます。

`Root` は不透明な借用版ハンドルです。コピーは O(1) でノードを共有しますが、公開済みノードは変更しません。更新は新しい版を返し、検索は入力版を変更せず、ノードも確保しません。管理オブジェクトはコピー禁止・move可能で、版は移動先で引き続き使用できます。管理領域の破棄・move代入による置換後は、その領域の版を使用できません。別の管理領域の版を混ぜる操作は不可で、デバッグ時には所有元を検査します。移動元の管理オブジェクトは破棄または再代入してください。 leader/same/sizeにはrootを最後に渡す従来の引数順も残します。版の型名はRootに統一し、互換用の型エイリアスは提供しません。

## インターフェース

```cpp
#include "library/data_structure/persistent_dsu.hpp"
class PersistentDSU;
using Root = /* opaque handle */;
PersistentDSU();
PersistentDSU(int n);
Root init(int n);
Root empty_root() const;
int leader(Root root, int i) const;
bool same(Root root, int i, int j) const;
int size(Root root, int i) const;
Root merge(Root root, int i, int j);
std::pair<int,Root> merge(int i, int j, Root root); // 正しい新代表も返す
std::size_t allocated_nodes() const;
```

## 計算量

頂点数N。検索・併合は最悪 O(log N · log₂₀(N+1)) 時間。mergeは O(log₂₀(N+1))ノードを追加します。初期化は O(N log₂₀(N+1)) 時間・O(N+1)ノード。版のコピーと初期版の取得は O(1)。

## 依存関係

- [persistent_vector.hpp](persistent_vector.md)

## Verify・利用例

- [unit/pointer_structures](../../../verify/unit/pointer_structures.cpp)
- [library_checker/persistent_unionfind](../../../verify/library_checker/data_structure/persistent_unionfind.cpp)
