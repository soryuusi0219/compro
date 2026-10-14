# Library Checker verification

[`yosupo06/library-checker-problems`](https://github.com/yosupo06/library-checker-problems)
が生成する公式テストケースと output checker を使って、自作ライブラリを検証します。

## Requirements

- Python 3.8 以上
- `g++` または `CXX` で指定した C++ コンパイラ
- Git

色付きログを利用する場合のみ、任意で `colorlog` をインストールできます。

## Run

全ターゲットを検証します。

```sh
make verify
```

ターゲットを指定することもできます。

```sh
make verify TARGETS=point_add_range_sum
```

自作ライブラリを埋め込み、AtCoder Libraryのincludeを残した提出用コードは、次のコマンドで
`build/expanded/verify/library_checker/` に生成できます。

```sh
make expand-verify
```

生成したコードを公式ケースで検証する場合は `--expanded` を指定します。この場合は
リポジトリのincludeパスを渡さず、AtCoder Libraryのincludeパスだけをコンパイラへ渡します。

```sh
make all
make verify VERIFY_ARGS=--expanded
```

利用できるターゲットを表示するには次を実行します。

```sh
make verify-list
```

初回実行時は `config.json` で固定した Library Checker Problems を
`build/library-checker-problems/` に clone します。生成済みテストケースを再利用する場合は
`--no-generate`、ネットワークアクセスを禁止する場合は `--offline` を指定します。

## Add a target

1. 問題の入出力と `library/` を接続する C++ コードを分野別ディレクトリへ追加します。
2. `manifests/<分野>/<target名>.json` に名前、Library Checker の問題パス、
   ソースパスを記述します。分野ディレクトリはverifyソースと同じ階層にします。
3. `make verify TARGETS=<name>` を実行します。

```json
{
  "name": "point_add_range_sum",
  "problem": "data_structure/point_add_range_sum",
  "source": "verify/library_checker/data_structure/point_add_range_sum.cpp"
}
```

失敗した入力と実際の出力は `build/verify/failures/<name>/` に保存されます。
Library Checker Problems の更新は `config.json` の `revision` と
GitHub Actions のキャッシュキーを同じコミットへ変更して行います。
