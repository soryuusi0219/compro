# segtree_beats

[索引](../README.md) / [実装](../../../library/data_structure/segtree_beats.hpp)

## 機能・前提条件

mapping が適用不能なら fail を返して再帰するSegment Tree Beats本体。償却計算量は使うS/Fの設計に依存する。

## インターフェース

```cpp
#include "library/data_structure/segtree_beats.hpp"
```

主要な宣言（補助型・補助関数を含む抜粋）:

```cpp
template<class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
struct segtree_beats;
segtree_beats(int n_ = 0);
segtree_beats(const vector<S>& v);
void set(int p, S x);
S get(int p);
S prod(int l, int r);
S all_prod();
void apply(int p, F f);
void apply(int l, int r, F f);
void all_apply(F f);
int max_right(int l, auto g);
int min_left(int r, auto g);
```

## 計算量

通常の一点・集約 O(log N)、更新は失敗回数に依存し最悪 O(N)。空間 O(N)。

## 詳細な使用方法

既存の簡易referenceを統合した内容です。

基本は lazy_segtree と同じ。

S のメンバ変数 fail が存在し、写像が適用不可能な時に fail=true を返す。


segtree_beats_templates.hpp のクラス:

    SegtreeBeatsChminChmaxAdd<T, Sum = T>: chmin / chmax / add
    SegtreeBeatsChminAdd<T, Sum = T>:      chmin / add
    SegtreeBeatsChmaxAdd<T, Sum = T>:      chmax / add

各クラスは segtree_beats を継承し、chmin / chmax / add は派生クラスに実装する。

T は要素・更新値、Sum は区間和の型。加算や遅延操作の合成は T に収まる範囲で使う。

どの版も区間和 sum、最小値 mn1、最大値 mx1 を取得できる。

片側版は不要な側の第2値・個数を保持しない。


使用例:

    #include "library/data_structure/segtree_beats_templates.hpp"

    using Seg = soryuusi::SegtreeBeatsChminChmaxAdd<int, long long>;
    std::vector<int> a = {1, 5, 3};
    Seg seg(a);

    seg.chmin(0, 3, 4); // {1, 4, 3}
    seg.chmax(0, 2, 2); // {2, 4, 3}
    seg.add(1, 3, 1);   // {2, 5, 4}
    auto result = seg.prod(0, 3);
    // result.sum == 11, result.mn1 == 2, result.mx1 == 5

S(x) は値 x を持つ1要素、S() は長さ0の単位元。

S = Seg::value_type として、初期値は `vector<T>` と `vector<S>` のどちらでも渡せる。

`vector<T>` は各要素を S(x) に変換して構築する。set(i, S(x)) での設定も可能。

seg(n) の初期状態は各葉が S() なので、全要素を set してから使う。


chmin(l, r, x)、chmax(l, r, x)、add(l, r, x) で半開区間 [l, r) を更新できる。

片側版では対応する chmin または chmax と add のみ利用できる。

従来の apply も利用可能。F = Seg::action_type として、

F::chmin(x)、F::chmax(x)、F::add(x) で操作を作る。

apply(l, r, f) は半開区間 [l, r)、apply(p, f) は1点、all_apply(f) は全体に適用する。

## 依存関係

標準ライブラリのみ。

## Verify・利用例

直接includeするverifyはありません。依存先からの間接利用は `make report` で確認できます。
