# Library Checker verification

[`online-judge-tools/verification-helper`](https://github.com/online-judge-tools/verification-helper)
の `oj-verify` を使い、Library Checker のシステムケースで
自作ライブラリを検証します。

## Requirements

- Python 3.8 以上
- `g++` または `CXX` で指定した C++ コンパイラ
- `requirements-verify.txt` に記載した Python パッケージ

```sh
python3 -m pip install -r requirements-verify.txt
```

## Run

全ターゲットを検証します。

```sh
make verify
```

ターゲットを指定することもできます。

```sh
make verify TARGETS=library_checker/point_add_range_sum
```

自作ライブラリを埋め込み、AtCoder Libraryのincludeを残した提出用コードは、次のコマンドで
`build/expanded/verify/library_checker/` に生成できます。

```sh
make expand-verify
```

生成したコードを公式ケースで検証する場合は `--expanded` を指定します。
verification-helper用の `.test.cpp` は展開後のソースを読み込みます。

```sh
make all
make verify VERIFY_ARGS=--expanded
```

利用できるターゲットを表示するには次を実行します。

```sh
make verify-list
```

`tools/verify.py` はmanifestから `build/verify/oj/sources/` に `.test.cpp` を生成し、
`oj-verify run` へ渡します。取得したシステムケースはverification-helperにより
`.verify-helper/cache/` へキャッシュされます。並列数、全体のタイムアウト、各ケースの
制限時間はそれぞれ `-j`、`--timeout`、`--tle` で指定できます。

## Add a target

1. 問題の入出力と `library/` を接続する C++ コードを分野別ディレクトリへ追加します。
2. `manifests/<分野>/<target名>.json` に名前、Library Checker の問題パス、
   ソースパスを記述します。分野ディレクトリはverifyソースと同じ階層にします。
3. `make verify TARGETS=library_checker/<name>` を実行します。

```json
{
  "name": "point_add_range_sum",
  "problem": "data_structure/point_add_range_sum",
  "source": "verify/library_checker/data_structure/point_add_range_sum.cpp"
}
```

manifest内の`name`には接頭辞を付けません。公開されるverifyターゲット名は、
配置先から自動的に`library_checker/<name>`になります。

`problem` の末尾から Library Checker の問題URLを生成します。
verification-helperのバージョンは `requirements-verify.txt` で固定しています。
