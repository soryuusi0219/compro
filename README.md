# compro

競技プログラミングで使用する C++ ライブラリ、解答テンプレート、検証コード、
補助ツールをまとめたリポジトリです。

## 必要な環境

- GNU Make
- Python 3
- C++20 に対応した `g++`
- Git（検証用リポジトリの取得に使用）

利用できる Make ターゲットは、リポジトリルートで次のコマンドを実行すると確認できます。

```sh
make help
```

## ディレクトリ構成

```text
.
├── library/       # 自作ライブラリ
├── templates/     # 解答・tester のテンプレート
├── third_party/   # AtCoder Library などの外部ライブラリ
├── verify/        # Library Checker 用の検証コードと設定
├── tools/         # 依存関係管理、展開、検証用ツール
├── misc/          # 問題固有のコードや補助ファイル
├── work/          # make contest で作る作業ディレクトリ
├── contest.json   # コンテスト作成時の既定設定
└── Makefile       # 各ツールのエントリーポイント
```

`build/` と `work/` の内容は生成物であり、Git の管理対象外です。

## 基本的な使い方

よく使うコマンドは次のとおりです。

```sh
make check
make verify-list
make verify TARGETS=convolution_mod
make expand path/to/main.cpp
make all
```

### ライブラリを読み込む

自作ライブラリは、リポジトリルートからのパスで読み込みます。

```cpp
#include "library/data_structure/bit.hpp"
```

直接コンパイルする場合は、このリポジトリと AtCoder Library をインクルードパスへ追加します。

```sh
g++ -std=c++20 -O2 -I. -Ithird_party/ac-library source.cpp
```

ライブラリのファイル名は、小文字の `snake_case` を基本としています。

## 解答コードのビルドと展開

ソースは、位置引数、`SRC`、または `FILE` で指定できます。

```sh
make run main.cpp
make expand main.cpp

make run SRC=work/abc123/A.cpp
make expand FILE=work/abc123/A.cpp
```

- `make run`: `_DEBUG` を有効にしてコンパイルし、提出用コードを生成してから実行します。
- `make expand`: 自作ライブラリを展開した提出用コードだけを生成します。

例えば `A.cpp` を指定した場合、提出用コードは同じディレクトリの
`submit_A.cpp` に出力されます。既定の展開ではデバッグ部分、コメント、空行を除去します。

AtCoder Library の扱いは、ソースと同じディレクトリにある `.contest.json` で決まります。

- `"acl_available": true`: `#include <atcoder/...>` を提出用コードに残します。
- `"acl_available": false`: AtCoder Library も提出用コード内へ展開します。

## コンテスト用ディレクトリを作る

リポジトリ内の `work/` にコンテスト用ディレクトリを作成できます。

```sh
make contest NAME=abc123
make contest NAME=abc123 PROBLEM_COUNT=6
make contest NAME=abc123 ACL=false
make contest NAME=abc123 TEMPLATE=templates/custom.cpp
make contest NAME=abc123 PROBLEMS=ABCDEF
```

主な引数は次のとおりです。

| 引数 | 内容 | 既定値 |
| --- | --- | --- |
| `NAME` | `work/` 以下に作るディレクトリ名 | 必須 |
| `PROBLEM_COUNT` | 作成する問題数 | `12` |
| `PROBLEMS` | 問題名を直接指定する文字列 | 自動生成 |
| `ACL` | AtCoder Library が利用可能か | `true` |
| `TEMPLATE` | 解答テンプレート | ACL 設定に応じて選択 |
| `TESTER_TEMPLATE` | tester のテンプレート | `templates/tester.cpp` |

既定値は [`contest.json`](contest.json) で変更できます。`ACL=true` の場合は
`templates/main.cpp`、`ACL=false` の場合は `templates/main_noACL.cpp` を使用します。

生成される主なファイルは次のとおりです。

```text
work/abc123/
├── A.cpp ...
├── tester.cpp
├── .contest.json
└── Makefile
```

既存ファイルは上書きしません。生成後は、そのディレクトリ内でも Make コマンドを使えます。

```sh
cd work/abc123
make run A.cpp
make expand A.cpp
```

## ライブラリの依存関係と生成物

自作ライブラリ間の依存切れ、循環依存、AtCoder Library への依存を検査します。

```sh
make check
```

依存を展開したライブラリ、提出用の verify コード、検証状況レポートをまとめて生成します。

```sh
make all
```

個別に生成することもできます。

```sh
make expand-library
make expand-library HEADERS=library/data_structure/bit.hpp
make expand-verify
make expand-verify TARGETS=convolution_mod_1000000007
make report
```

生成先は次のとおりです。

- 展開済みライブラリ: `build/expanded/library/`
- 展開済み verify コード: `build/expanded/verify/library_checker/`
- 検証状況レポート: `build/library_status.md`、`build/library_status.json`

## 検証

登録済みライブラリは、固定バージョンの `yosupo06/library-checker-problems` の
公式テストケースと checker で検証できます。必要なリポジトリは初回実行時に
`build/` 以下へ取得されます。

```sh
make verify-list
make verify
make verify TARGETS=convolution_mod
make verify VERIFY_ARGS=--expanded
```

対象の追加方法や設定の詳細は
[`verify/library_checker/README.md`](verify/library_checker/README.md) を参照してください。
ツールごとの詳しい引数は [`tools/README.md`](tools/README.md) にまとめています。
