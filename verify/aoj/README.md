# AOJ verification

[`online-judge-tools/verification-helper`](https://github.com/online-judge-tools/verification-helper)
の `oj-verify` を使い、AOJ の公開テストケースで自作ライブラリを検証します。

## Add a target

1. 問題の入出力と `library/` を接続する C++ コードを分野別ディレクトリへ追加します。
2. `manifests/<分野>/<target名>.json` に名前、問題識別子、AOJ の問題URL、
   ソースパスを記述します。分野ディレクトリはverifyソースと同じ階層にします。
3. `make verify TARGETS=aoj/<name>` を実行します。

```json
{
  "name": "range_min_segtree_template",
  "problem": "aizu/DSL_2_A",
  "url": "https://onlinejudge.u-aizu.ac.jp/problems/DSL_2_A",
  "source": "verify/aoj/data_structure/range_min_segtree_template.cpp"
}
```

manifest内の`name`には接頭辞を付けません。公開されるverifyターゲット名は、
配置先から自動的に`aoj/<name>`になります。

必要なパッケージ、実行方法、展開済みソースの検証方法は
[`../library_checker/README.md`](../library_checker/README.md) と共通です。
