# permutation_subgroup

[索引](../README.md) / [実装](../../../library/math/permutation_subgroup.hpp)

## 機能・前提条件

`[0,n)` 上の置換を生成元として部分群を構築し、群の位数の因子と置換の所属を求める。置換 `p` は長さ `n` の配列で、`p[i]` が点 `i` の像を表す。各要素は `[0,n)` 内で一度ずつ現れること。合成 `p ∘ q` の像は `p[q[i]]`。

`G_k` を生成した群のうち `k` より大きい点を全て固定する部分群とする。点 `n-1` から順に固定していく安定化部分群列を使う。`indices()[k]` は `G_k` による点 `k` の軌道サイズ、すなわち指数 $[G_k:G_{k-1}]$。ここで `G_{-1}` は恒等置換だけを含む。全因子の積が群の位数になる。整数型の範囲を超える場合は任意精度整数で積を取るか、必要な法で積を取る。

## インターフェース

```cpp
#include "library/math/permutation_subgroup.hpp"

namespace soryuusi {
struct permutation_subgroup {
  permutation_subgroup(int n);
  permutation_subgroup(int n, std::vector<std::vector<int>> generators);
  void add(std::vector<int> p);
  void build();
  std::vector<int> indices() const;
  bool is_in(std::vector<int> p) const;
};
}
```

- `n >= 0`。コンストラクタまたは `add(p)` で生成元を登録し、`build()` で安定化部分群列を構築する。
- 構築後にも生成元を追加できる。`build()` は既存の列を引き継ぎ、登録した置換が既に属している場合はその置換の構築処理を省く。再構築では新たな生成元・代表元が関わる積を調べる。
- `indices()` と `is_in(p)` は `build()` の後に呼ぶ。`is_in` は `p` を生成元とその逆置換の合成で表せる場合に `true` を返す。
- 生成元が空の場合の群は恒等置換だけを含む。`n == 0` の位数は空積の1。

## 計算量

Knuthによる決定的なSims法を使う。各段は軌道の代表元と、その段の群を生成する置換の列を保持する。代表元と生成元の積をふるい分け、軌道に新しい像を加えるか、1つ下の群に不足する生成元を渡す。既に処理した積は再計算しない。[Knuth, Efficient Representation of Perm Groups, §2–4](https://arxiv.org/abs/math/9201304)

各段で保持する代表元数と採用する生成元数はともに $O(k+1)$。生成元はその段の群を真に拡大するときだけ採用し、対称群の部分群列の長さの上界を使う。積1組のふるい分けは最悪 $O((k+1)^2)$ なので、全段を通した構築は $O(n^5)$。登録した $K$ 個の置換の所属判定に $O(Kn^2)$ が加わる。[Knuth, §4](https://arxiv.org/pdf/math/9201304#page=3)

| 操作 | 最悪時間 |
| --- | --- |
| 空の構築 | $O(n)$ |
| `add` | 償却 $O(n)$ |
| `is_in` | $O(n^2)$ |
| 全生成元の登録と `build` の合計 | $O(n^5+Kn^2)$ |
| `indices` | $O(n)$ |

表と作業領域は $O(n^3)$。1回の `build()` に向けて登録した生成元数の最大を $B$ とすると、保留する配列に $O(Bn)$ を使う。各段の置換を連続領域に格納し、合成用の領域を再利用する。

小さい点からの接頭辞が対称群全体を含むと確定したら、その範囲のふるい分けを省く。交代群全体を含む場合は、残った積が偶置換なら同様に省ける。入力の置換から累乗で互換を取り出せる場合は、生成元による共役で得た互換が全点をつなぐかも調べる。つながれば対称群全体であることが確定する。連結成分を併合する互換だけをキューに置くため、この判定は1回の `build()` で $O(Bn\alpha(n))$ 時間。

## 依存関係

- [dsu.hpp](../data_structure/dsu.md)

## Verify・利用例

```cpp
soryuusi::permutation_subgroup group(3, {{1, 2, 0}, {1, 0, 2}});
group.build();
auto factors = group.indices();  // {1, 2, 3}
bool contains = group.is_in({0, 2, 1});  // true
```

- [`library_checker/permutation_group_order`](../../../verify/library_checker/other/permutation_group_order.cpp)
- [`unit/permutation_subgroup`](../../../verify/unit/permutation_subgroup.cpp)

## 実行時間の比較

Intel Core i7-12700の同一論理CPUでGCC 15.2、`-std=c++20 -O2 -DNDEBUG` を使用。各入力で構築・全生成元の登録・`build()`・`indices()` を計時した。入出力と破棄を計時から除き、各プロセス内で反復した中央値を3プロセス分取得し、その中央値を比較した。公式50ケースは旧版・新版とも出力が公式解と一致した。公式ケースごとの計測値の中央値は 64.0 → 8.85 µs、最大値は 310 → 3.33 ms。[Library Checkerのケース設定](https://github.com/yosupo06/library-checker-problems/blob/9e8d25b9fe1b8eac878b3e3b6a9d205a06b6f653/other/permutation_group_order/info.toml)

| 入力 | 変更前 → 変更後 |
| --- | ---: |
| 公式 `random_10`（50点の交代群、生成元2個） | 304 → 3.33 ms |
| 公式 `random_11`（50点の対称群、生成元100個） | 310 → 0.00331 ms |
| 公式 `structured_02`（49点の交代群と固定点） | 108 → 0.208 ms |
| 公式 `structured_05`（独立な互換25個） | 16.9 → 45.4 µs |
| 50点の恒等置換100個 | 3.03 → 2.58 µs |
| 80点で接頭辞を順次広げる生成元79個 | 4.25 s → 7.58 µs |

最後の行は、最も大きい移動点が1点ずつ増えるように生成元を並べ、各接頭辞で対称群を生成する補助入力。
