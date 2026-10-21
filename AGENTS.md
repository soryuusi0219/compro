# AGENTS.md

このファイルはリポジトリ全体に適用する作業指針です。競技プログラミング用ライブラリなので、
汎用アプリケーション向けの抽象化より、短く予測可能な実装、実行時間、メモリ使用量、提出時の
展開しやすさを重視してください。

## リポジトリ概要

- C++20 の競技プログラミング用ライブラリです。
- `library/`: 分野別のヘッダーオンリーライブラリ
- `verify/library_checker/`: Library Checker 用の接続コードと `manifests/`
- `verify/unit/`: 公式問題がない機能、細かなAPI、ランダム比較のテスト
- `templates/`: 解答コードとtesterのテンプレート
- `tools/`: include展開、依存関係検査、verify、コンテスト作成ツール
- `third_party/`: AtCoder Libraryなどの外部コード
- `build/`, `work/`: 生成物。原則として編集・コミットしません

## 作業前の確認

- 最初に `git status --short` を確認してください。未コミット差分は利用者の作業として扱い、
  関係のない変更を戻したり整形したりしないでください。
- 対象ヘッダーだけでなく、既存のverify、ドキュメント、利用箇所を `rg` で確認してください。
- API変更を依頼されていない限り、既存のコンストラクタ、関数名、デフォルト引数、戻り値を
  維持してください。新しい表現へ対応するときも従来のインターフェースを残します。
- 変更は要求を満たす最小範囲に留め、周辺コードの一括整形や無関係なリファクタリングは
  避けてください。

## ライブラリの書式と実装方針

- `library/**/*.hpp` の1行目は次の形式でなければなりません。`make check` が検査します。

  ```cpp
  // https://github.com/soryuusi0219/compro/blob/main/library/<relative-path>.hpp
  ```

- 続けて `#pragma once` を置き、必要な標準ヘッダーと直接依存する自作ヘッダーを明示的に
  includeしてください。推移的includeには依存しません。
- 自作ライブラリは原則 `namespace soryuusi` 内に置きます。既存ファイルでは
  `using namespace std;`、空白、括弧、命名など、そのファイルのスタイルに合わせてください。
- ファイル名は小文字の `snake_case` を基本とします。既存の型名や歴史的な綴りは、明示的な
  改名依頼がない限り維持してください。
- 型は制約と既存APIに合わせます。頂点番号や配列添字は基本的に `int` とし、必要のない
  `long long` 化は避けてください。
- 再帰処理には型消去を伴う `std::function` ではなく、原則として次の再帰ラムダを使います。

  ```cpp
  auto dfs = [&](auto&& self, int v) -> void {
    // ...
    if(/* recurse */) self(self,child);
  };
  ```

- 実装はコンパクトに保ちますが、未定義動作、型変換の破綻、APIの曖昧さを生む短縮は避けます。
- `third_party/` は外部コードです。依頼がない限り変更しません。

## Verifyの追加と実行

- 変更した機能には必ず対応するverifyを実行してください。既存ターゲットがなければ、適切な
  Library Checkerターゲットを追加するか、`verify/unit/` に決定的テスト／ランダム比較テストを
  追加します。
- ユニットテストは通常、次の警告設定でコンパイルして実行します。

  ```sh
  g++ -std=c++20 -O2 -Wall -Wextra -Werror -I. verify/unit/<test>.cpp -o /tmp/<test>-test
  /tmp/<test>-test
  ```

- AtCoder Libraryを直接使う場合は `-Ithird_party/ac-library` も指定します。
- Library Checkerターゲットは次の組で構成します。
  - `verify/library_checker/<category>/<name>.cpp`
  - `verify/library_checker/manifests/<category>/<name>.json`
- 登録済みターゲットは次のように実行します。

  ```sh
  make verify TARGETS=<target>
  # 生成済みケースをネットワークなしで再利用する場合
  python3 tools/verify.py <target> --offline --no-generate
  ```

- 変更後は少なくとも `make check` と `git diff --check` を実行してください。
  `make check` は先頭URL、依存切れ、循環依存などを検査します。
- include展開や依存関係への影響が大きい変更では、必要に応じて以下も実行します。

  ```sh
  make expand-verify TARGETS=<target>
  make verify TARGETS=<target> VERIFY_ARGS=--expanded
  ```

- 全verifyは重いため、まず変更対象へ直接または推移的に依存するターゲットを実行します。
  `make affected BASE=<revision>` で候補を確認できます。
- 利用者が既に変更していた関連箇所も含め、現在のワークツリー全体としてverifyしてください。
  失敗が変更と無関係なら、対象ファイルが未変更であることを確認した上で結果に明記します。

## ドキュメントと互換性

- 公開テンプレート引数、戻り値、単位元、構築手順が変わる場合は、対応する
  `*_document.txt`、`README.md`、サンプルも更新してください。
- デフォルトテンプレート引数を追加・変更する場合は、従来の省略形がコンパイルできることと、
  明示指定した形の両方をテストしてください。
- `fastio.hpp` と通常のiostreamの双方から利用される入出力演算子では、独自パーサを重複実装せず、
  既存の整数入出力との互換性を優先してください。

## 完了時の報告

- 変更した主要ファイルと公開APIへの影響を簡潔にまとめてください。
- 実行したユニットテスト、Library Checkerターゲット、`make check` の結果を報告してください。
- ベンチマークを行った場合は、入力、回数、変更前後の値、採用判断を記載してください。
- テスト未実行、既存不具合、環境上の制約があれば隠さず明記してください。
