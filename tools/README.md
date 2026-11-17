# Tools

ライブラリの展開や検証を自動化する、リポジトリ共通のツールを置きます。
内部設計と開発手順は [architecture.md](../architecture.md) を参照してください。

- `verify.py`: 成功記録を照合し、必要なオンラインジャッジ・unitターゲットを実行します。オンライン実行は`verify_online.py`がverification-helperの実行エンジンを呼び出します。
- `library_manager.py`: 自作ライブラリ・AtCoder Library・verifyの依存関係確認、依存を展開した単一ファイル生成、verify状況レポート生成を行います。
- `expand_include.py`: 解答ソースの自作ライブラリと、設定に応じてACLを再帰展開します。
- `create_contest.py`: 設定したテンプレートから問題ファイル、tester、Makefileを作成します。

```sh
# 依存関係・referenceの検査とツールの回帰テスト
make check

# build/expanded/library/ 以下へ全ヘッダーを展開
make expand-library

# build/expanded/verify/{library_checker,aoj,unit}/ 以下へverifyコードを展開
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

特定のverifyターゲットだけを展開する場合は、種別を含むターゲット名を指定します。

```sh
make expand-verify TARGETS=library_checker/convolution_mod_1000000007
make expand-verify TARGETS=unit/rolling_hash
```

自作ライブラリは生成ファイル内へ再帰的に展開します。AtCoder Libraryは展開せず、`#include <atcoder/...>` を残します。依存関係の検査とレポートにはAtCoder Libraryも含まれます。展開済みverifyコードは、対応するオンラインジャッジへファイル全体をコピーして提出できます。

ヘッダー内の相対includeは、そのヘッダーのディレクトリから解決します。依存グラフ・レポートでは
`library/...` に正規化して扱います。解答・verifyのルート基準includeも認識します。
ヘッダー内でのルート基準includeは `make check` でエラーになります。
依存解決と展開の回帰テストは `python3 -m unittest discover -s tools -p 'test_*.py'` で実行できます。

展開済みコードを公式テストケースで検証する場合は `--expanded` を指定します。対象は自動で再展開します。

```sh
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

CIはmainへのpush、PR、手動実行で起動します。全ターゲットの入力を照合し、成功記録のないものだけを
matrixへ1件ずつ割り当て、最大8件を並列実行します。PRではcheckoutされたmerge結果を検証します。
コンテナの後続shellからもGitを使えるよう、checkout先だけを`safe.directory`へ登録します。
同じPR・refの古いworkflowはキャンセルします。依存関係・reference・ツールのテストは毎回実行します。

## 成功記録と再実行

`make verify` は `build/verify/results/records/<SHA256>.json` に成功記録を保存します。
ソース・推移的include・manifest・コンパイル条件・実行環境・テスト条件が一致すると省略します。
compilerの`-MM`で依存を取得するため、`templates/`、マクロinclude、各`TEMPLATE_KIND`も対象です。
実行時に読むファイルがあれば、オンラインmanifestの`dependencies`にリポジトリ相対パスを列挙します。

```sh
make verify-plan
make verify-plan TARGETS=unit/rolling_hash VERIFY_ARGS=--expanded
make verify TARGETS=unit/rolling_hash VERIFY_ARGS=--force
make verify TARGETS=library_checker/primality_test_internal_math VERIFY_ARGS=--refresh-cases
```

`--plan` はテストを走らせず、実行予定をstdout、RUN/SKIPと理由をstderrへ出します。
`--format github`を付けるとmatrix/countを出力します。差分SHAが不明でもこの照合は使えます。
`make affected`は差分の説明用です。比較元が空・ゼロ・取得不能ならmainとのmerge-baseを試し、
それも不明なら全件を候補にします。候補が全件でも、検証キーが一致すれば実行は不要です。

`--force`は成功記録を無視します。`--refresh-cases`は強制実行に加えてケースを再取得します。
`--expanded`は対象を自動で再展開し、通常コンパイルとは別の成功記録を作ります。
`VERIFY_CXXFLAGS`で検証用の追加オプション、`CXX`でコンパイラを指定できます。
`--tle`はオンラインのケース当たり上限（既定60秒）、`--timeout`はオンラインターゲットの全体上限、
unitでは各コンパイル・実行プロセスの上限です。どちらも秒単位です。

失敗・中断・IGNORE・ケース未完了は成功記録を作りません。途中失敗があっても、完了した別ターゲットの
成功は保存します。検証中に入力が変わった場合も成功扱いにしません。helperの時刻によるスキップは使いません。
強制再実行で失敗・中断した場合は同じキーの古い成功を無効化し、同期で復活しないよう無効化記録も共有します。

## ローカルとCIの結果を同期する

共有先は`origin`の専用ブランチ`verify-results`です。ソースのブランチ・index・ワークツリーは変更しません。
明示的に同期する場合だけGitリモートへアクセスします。

```sh
make verify-sync                                      # CIなどの成功記録を取得
make verify TARGETS=unit/rolling_hash VERIFY_ARGS=--expanded
make verify-sync VERIFY_ARGS='--sync push'             # ローカルの成功記録を共有
```

`--remote`、`--results-branch`、`--results-dir`で保存先を変更できます。
初回の取得時に共有ブランチがなくても正常終了し、最初のpushで作成します。
push時は既存の記録と統合し、競合時は取り直して再試行します。force-pushは使いません。
CIは成功記録をartifactに保存してから集約・公開します。fork PRの記録は共有ブランチへ公開しません。
リモートの通信・認証エラーは明示し、成功記録が取得できたことにはしません。

環境とビルド方式が異なる結果は再利用しません。CIは`--expanded`を使うため、共有したいローカル実行でも指定します。
CIと同じUbuntu環境をローカルで使う場合は、次のコンテナを使えます。
パッケージ更新で実際のバージョンが異なる場合は再検証されます。

```sh
docker build -f tools/verify.Dockerfile -t compro-verify .
docker run --rm --user "$(id -u):$(id -g)" -e HOME=/tmp -v "$PWD:/repo" compro-verify \
  python tools/verify.py --expanded unit/rolling_hash
make verify-sync VERIFY_ARGS='--sync push'
```

記録は信頼する利用者・CIが公開する運用です。ソースのSHAは追跡情報であり、照合キーには使いません。

## テストケースの版

[verify/cases.json](../verify/cases.json)でLibrary Checkerのコミット、AOJのepoch、有効期間を管理します。
Library Checkerは指定コミットを取得し、自動pullで別の版へ変わらないようにします。
`VERIFY_LIBRARY_CHECKER_REPOSITORY`で既存のクリーンな指定コミットのリポジトリも使えます。
AOJは更新版番号が得られないためepochを手動更新できるようにし、成功記録と取得済みケースは30日で失効します。
Library Checkerにも同じ有効期間を適用します。ケースとcheckerは固定版から再確認します。
取得ケースのダイジェストを照合し、破損・版違い・期限切れなら再取得します。
生成したケースの内容と完了状況を成功記録のevidenceへ保存します。

`build/library_status.*`のdirect/indirect/unverifiedは接続コードの有無による網羅状況です。
実際の成功・失敗の実績とは区別してください。実行要否と成功記録は`make verify-plan`で確認します。

問題・コンテスト固有のスクリプトは `misc/` に置きます。
## Referenceの検査

ライブラリの説明は [reference索引](../docs/reference/README.md) にあります。
`python3 tools/check_references.py` は全ヘッダーの対応ページ、必須節、索引登録、
ローカルリンクを検査します。この検査は `make check` にも含まれます。
APIの意味・型制約・計算量の正しさは自動判定しないため、実装・verifyと合わせて確認してください。
