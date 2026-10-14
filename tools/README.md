# Tools

ライブラリの展開や検証を自動化する、リポジトリ共通のツールを置きます。

- `verify.py`: Library Checker Problems の取得、テスト生成、解答のコンパイル、checker 判定を一括実行します。
- `library_manager.py`: 自作ライブラリ・AtCoder Library・verifyの依存関係確認、依存を展開した単一ファイル生成、verify状況レポート生成を行います。
- `expand_include.py`: 解答ソースの自作ライブラリと、設定に応じてACLを再帰展開します。
- `create_contest.py`: 設定したテンプレートから問題ファイル、tester、Makefileを作成します。

```sh
# 依存切れと循環依存を検査
make check

# build/expanded/library/ 以下へ全ヘッダーを展開
make expand-library

# build/expanded/verify/library_checker/ 以下へ提出用コードを展開
make expand-verify

# build/library_status.{json,md} を生成
make report

# 上記をまとめて実行
make all

# 2つのgit revision間で影響を受けるverifyターゲットを表示
make affected BASE=origin/main
```

特定のヘッダーだけを展開する場合は、リポジトリルートからのパスを指定します。

```sh
make expand-library HEADERS=library/data_structure/wavelet_bit.hpp
```

特定のverifyターゲットだけを展開する場合は、manifestのターゲット名を指定します。

```sh
make expand-verify TARGETS=convolution_mod_1000000007
```

自作ライブラリは生成ファイル内へ再帰的に展開します。AtCoder Libraryは展開せず、`#include <atcoder/...>` を残します。依存関係の検査とレポートにはAtCoder Libraryも含まれます。展開済みverifyコードは、AtCoder Libraryを利用できるLibrary Checkerへファイル全体をコピーして提出できます。

展開済みコードを公式テストケースで検証する場合は、先に展開してから `--expanded` を指定します。

```sh
make all
make verify VERIFY_ARGS=--expanded
```

コンテスト用ディレクトリは `make contest NAME=abc123` で `work/abc123/` に作成します。
`contest.json` でACLあり・なしのテンプレート、tester、既定問題数、ACLの既定値を設定できます。

```sh
make contest NAME=abc123 PROBLEM_COUNT=6 ACL=false
make contest NAME=abc123 TEMPLATE=templates/custom.cpp
```

ACLありではACLのincludeを保持し、ACLなしではACLも提出コードへ展開します。
生成先では `make run A.cpp` でコンパイル・展開・実行、`make expand A.cpp` で
`submit_A.cpp` の生成だけを行います。問題ソース、tester、設定ファイルなどの既存ファイルは上書きしません。

CIは変更されたverifyソース、`manifests/<分野>/<target名>.json`、および変更された自作ライブラリまたはAtCoder Libraryへ推移的に依存するターゲットだけを検証します。選ばれたターゲットはmatrix jobへ1件ずつ割り当て、最大8件を並列実行します。Library Checker Problemsは準備jobで取得してキャッシュし、各matrix jobから共用します。`tools/`、verifyの共通設定、CI workflowの変更時、および依存関係を現在のツリーから追跡できない関連ファイルの削除時は全ターゲットを検証します。依存関係チェックと展開は変更内容にかかわらず毎回実行します。

問題・コンテスト固有のスクリプトは `misc/` に置きます。
