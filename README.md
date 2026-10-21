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
├── verify/        # Library Checker・AOJ 別の検証コードと設定
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
make verify TARGETS=library_checker/convolution_mod
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
make expand-verify TARGETS=library_checker/convolution_mod_1000000007
make report
```

生成先は次のとおりです。

- 展開済みライブラリ: `build/expanded/library/`
- 展開済み verify コード: `build/expanded/verify/library_checker/`、
  `build/expanded/verify/aoj/`、`build/expanded/verify/unit/`
- 検証状況レポート: `build/library_status.md`、`build/library_status.json`

## 検証

登録済みライブラリは `online-judge-tools/verification-helper` の `oj-verify` を使った
Library Checker・AOJ の公開テストケースと、ローカルのunitテストで検証できます。
オンラインジャッジのターゲットを実行する場合は、初回のみ依存パッケージを
インストールしてください。

```sh
python3 -m pip install -r requirements-verify.txt
```

```sh
make verify-list
make verify
make verify TARGETS=library_checker/convolution_mod
make verify TARGETS=aoj/range_min_segtree_template
make verify TARGETS=unit/rolling_hash
make verify VERIFY_ARGS=--expanded
```

ターゲット名はそれぞれ `library_checker/<name>`、`aoj/<name>`、`unit/<name>` です。

対象の追加方法や設定の詳細は
[`verify/library_checker/README.md`](verify/library_checker/README.md) と
[`verify/aoj/README.md`](verify/aoj/README.md) を参照してください。
ツールごとの詳しい引数は [`tools/README.md`](tools/README.md) にまとめています。

## ライセンス

このリポジトリの内容は、特に明記がない限り
[`LICENSE`](LICENSE) に記載された CC0 1.0 Universal の下で提供されています。
許諾や事前連絡なしで、自由に使用・複製・改変・再配布できます。

ただし、`third_party/` 以下のファイルには、それぞれの著作者が定めるライセンスが
適用されます。利用する際は、各ファイルまたは各ディレクトリに記載された
ライセンス条件に従ってください。

soryuusi0219 が競技プログラミングで提出したコードについても、特に明記がない限り、
このリポジトリの内容と同様に CC0 1.0 Universal として扱います。ただし、他者から
引用した部分についてはこの限りではなく、引用元のライセンスおよび利用条件が
適用されます。

このリポジトリの内容および上記の提出コードには、バグやその他の不具合が含まれる
可能性があります。これらは無保証で提供され、利用によって生じたいかなる損害に
ついても、soryuusi0219 は一切の責任を負いません。
