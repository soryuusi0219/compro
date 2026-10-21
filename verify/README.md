# Verification

自作ライブラリをオンラインジャッジのテストケースで検証するコードを置きます。

現在は `online-judge-tools/verification-helper` を利用して Library Checker と
AOJ の公開テストケースを実行します。詳細は
[library_checker/README.md](library_checker/README.md) と [aoj/README.md](aoj/README.md) を
参照してください。

```text
verify/
├── library_checker/
│   ├── manifests/             # verifyソースと同じ分野別構造の検証設定
│   │   ├── data_structure/
│   │   ├── graph/
│   │   └── ...
│   ├── data_structure/
│   ├── graph/
│   └── ...
├── aoj/
│   ├── manifests/
│   ├── data_structure/
│   └── graph/
└── unit/                      # ローカルでコンパイル・実行するテスト
```

各検証コードは `library/` の実装を読み込み、問題固有の入出力と最小限の接続処理だけを持たせます。

## Unitテスト

`unit/` には、オンラインジャッジで直接検証できない細かなAPI、内部操作、
ランダム比較を残します。対応する実問題がある主要操作は
`library_checker/` または `aoj/` にもverifyを登録し、両方を独立して実行します。

各 `verify/unit/<name>.cpp` は `unit/<name>` として自動登録されます。個別または
全ターゲットを、オンラインジャッジ用ターゲットと同じコマンドで実行できます。

```sh
make verify TARGETS=unit/rolling_hash
make verify TARGETS='unit/segtree_template unit/lazy_segtree_template'
make verify
```

`unit/templates` は `TEMPLATE_KIND=0,1,2` の3構成を自動的にコンパイル・実行します。
利用できる名前は `make verify-list` で確認できます。

主要操作のオンラインジャッジ側への移行先は次のとおりです。

- Library Checker: independent set、汎用グラフ、CSR graph、rerooting、Euler Tour、
  link-cut tree、splay tree
- AOJ: BFS、modified min-cost-flow、segtree/lazy segtree template

suffix tree、rolling hash、Steiner treeには既存の Library Checker ターゲットが
あります。オンラインジャッジで表現しにくい互換別名、空区間、辺復元、
各コンストラクタなどはunitテストに残しています。

遅延セグメント木の全11テンプレートについて、更新・集約、空区間、
各コンストラクタ、カスタムINF/LOWEST、modint、遅延伝播後の二分探索を検証します。

suffix tree の suffix array、inverse suffix array、LCP、異なる部分文字列数、
suffix LCP、パターン出現数、matching state、最長共通部分文字列、node accessor は
ナイーブ実装とのランダム比較で検証します。

rolling hash の単一・多重ハッシュ、mod演算、部分文字列比較、LCP、`push_back` は
ナイーブ実装とのランダム比較で検証します。

文字列・数列の連結結果を辞書順最小にする `O(N log^2 N)` のソートは、
全順列とのランダム比較で検証します。

再帰ラムダを使うグラフ・木・link-cut tree・splay treeの各実装は、
専用テストで結果と内部操作を検証します。

平坦な隣接リストへ変更した modified min-cost-flow と Steiner tree は、
専用テストで逆辺、辺復元、両コンストラクタを検証します。

CSRグラフ本体、0-1 BFS・BFS・重心分解・offline dynamic connectivity、
およびCSR化したEuler Tour系も専用テストで検証します。

3種類のC++テンプレートの共通ヘルパーもunitターゲットで検証します。
`TEMPLATE_KIND` は順に main、main_noACL、tester を選びます。
tester 本体の無限ループは実行しません。

集約・ソートのマクロ、添字ソート、比較器、半開区間の判定、
出力ヘルパーの戻り値と出力、入れ子のハッシュキーを検証します。
