# Verification

自作ライブラリをオンラインジャッジのテストケースで検証するコードを置きます。

現在は `yosupo06/library-checker-problems` を利用します。詳細は
[library_checker/README.md](library_checker/README.md) を参照してください。

```text
verify/
└── library_checker/
    ├── config.json
    ├── manifests/             # verifyソースと同じ分野別構造の検証設定
    │   ├── data_structure/
    │   ├── graph/
    │   └── ...
    ├── data_structure/
    ├── graph/
    └── ...
```

各検証コードは `library/` の実装を読み込み、問題固有の入出力と最小限の接続処理だけを持たせます。

## ランダム比較テスト

`unit/` には、単純な配列と比較するローカルテストを置きます。
Library Checker のターゲットとは独立して実行します。

```sh
for test in segtree_template lazy_segtree_template; do
  g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. "verify/unit/$test.cpp" -o "/tmp/$test-test"
  "/tmp/$test-test"
done
```

遅延セグメント木の全11テンプレートについて、更新・集約、空区間、
各コンストラクタ、カスタムINF/LOWEST、modint、遅延伝播後の二分探索を検証します。

suffix tree の suffix array、inverse suffix array、LCP、異なる部分文字列数、
suffix LCP、パターン出現数、matching state、最長共通部分文字列、node accessor は
ナイーブ実装とのランダム比較で検証します。

```sh
g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. verify/unit/suffix_tree.cpp -o /tmp/suffix_tree_test
/tmp/suffix_tree_test
```

rolling hash の単一・多重ハッシュ、mod演算、部分文字列比較、LCP、`push_back` は
ナイーブ実装とのランダム比較で検証します。

```sh
g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. verify/unit/rolling_hash.cpp -o /tmp/rolling_hash_test
/tmp/rolling_hash_test
```

文字列・数列の連結結果を辞書順最小にする `O(N log^2 N)` のソートは、
全順列とのランダム比較で検証します。

```sh
g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. verify/unit/sort_concatenation_min.cpp -o /tmp/sort_concatenation_min_test
/tmp/sort_concatenation_min_test
```

再帰ラムダを使うグラフ・木・link-cut tree・splay treeの各実装は、
次の専用テストで結果と内部操作を検証します。

```sh
for test in graph_general independent_set link_cut recursive_tree splay_tree splay_tree_lazy; do
  g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. "verify/unit/$test.cpp" -o "/tmp/$test-test"
  "/tmp/$test-test"
done
```

平坦な隣接リストへ変更した modified min-cost-flow と Steiner tree は、
次の専用テストで逆辺、辺復元、両コンストラクタを検証します。

```sh
for test in min_cost_flow_modified steiner_tree; do
  g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. "verify/unit/$test.cpp" -o "/tmp/$test-test"
  "/tmp/$test-test"
done
```

CSRグラフ本体、0-1 BFS・BFS・重心分解・offline dynamic connectivity、
およびCSR化したEuler Tour系は、次の専用テストで検証します。

```sh
for test in static_graph csr_graph recursive_tree; do
  g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. "verify/unit/$test.cpp" -o "/tmp/$test-test"
  "/tmp/$test-test"
done
```

3種類のC++テンプレートの共通ヘルパーは、次のコマンドで検証します。
`TEMPLATE_KIND` は順に main、main_noACL、tester を選びます。
tester 本体の無限ループは実行しません。

```sh
for template_kind in 0 1 2; do
  g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. -Ithird_party/ac-library -DTEMPLATE_KIND="$template_kind" verify/unit/templates.cpp -o "/tmp/compro-template-test-$template_kind"
  "/tmp/compro-template-test-$template_kind"
done
```

集約・ソートのマクロ、添字ソート、比較器、半開区間の判定、
出力ヘルパーの戻り値と出力、入れ子のハッシュキーを検証します。
