# AGENTS.md

このファイルはリポジトリ全体に適用する作業指針です。競技プログラミング用ライブラリなので、
汎用アプリケーション向けの抽象化より、短く予測可能な実装、実行時間、メモリ使用量、提出時の
展開しやすさを重視してください。

## リポジトリ概要

- C++20 の競技プログラミング用ライブラリです。
- `library/`: 分野別のヘッダーオンリーライブラリ
- `docs/reference/`: 全ヘッダーに対応するMarkdown reference（機能・API・計算量・verify）
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
- 最初に [reference索引](docs/reference/README.md) から対象の説明を確認してください。
  `library/<category>/<name>.hpp` の説明は `docs/reference/<category>/<name>.md` です。
  説明と実装に差がある場合は、実装・verifyで確かめてreferenceも訂正してください。
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

## ポインタ木・永続構造の設計規約

- 可変ノードを所有する構造は原則コピー禁止・move可能にします。独立複製が必要なら
  計算量を明記した `clone()` を提供し、暗黙コピーで可変ノードを共有しないでください。
- ノード型・ルートポインタ・再確保できる内部配列は非公開にし、利用側は添字・頂点番号・
  列ハンドルで操作します。LCTは固定したノード配列を使い、別管理領域のノードを接続しません。
- 永続構造は管理オブジェクトと不透明な値型 `Root` を分離します。Rootは借用ハンドルで、
  版番号の管理は利用側に任せます。別管理領域のRootを混ぜる操作は禁止し、デバッグ時に検査します。
- 新しい版を返す更新は `[[nodiscard]]` とし、原則 `set(root,i,value)` のようにRootを先頭に渡します。
  検索はconstで、入力Root・公開済みノードを変更せず、ノードを確保しないでください。
  破壊的構築は未公開の木に限定し、可変ノード参照を外部に返しません。
- Rootの有効期間は管理領域の存続中です。管理オブジェクトのmove構築は版を引き継ぎ、
  領域の破棄・move代入による置換は元領域の版を無効化します。この契約をreferenceに記載します。
  再初期化で過去版の解釈が変わらないよう、永続DSUのinitは1回に限定します。
- split/concatなどの消費操作は、呼出元・消費元が空になることを明示します。同一プールでは
  所有領域だけを共有して可変部分木を分離し、異プールでは明示的な値の移送などで安全に扱います。
  `size()` は論理サイズ、`allocated_nodes()` は確保数として区別します。
- ノードごとのshared_ptr・deleteを標準方針にしません。安定アドレスのブロック型プール、
  または再確保に安全な整数IDを使い、領域単位の解放を基本とします。不要ノードの保持も許容します。
  ノード再利用は共有されていない可変ノードに限定し、公開済み永続ノードは再利用しません。
- 型名は通常版・遅延版・永続版を同時includeできるものにします。汎用的な `np` などを
  名前空間直下に追加しません。既存の衝突しない名前・引数順は可能な範囲で互換ラッパーを残します。
- 変更時は過去版からの分岐、検索前後のRoot・確保数不変、moveとclone、split/concat後の独立性、
  消費元破棄後の操作を検証します。借用proxy・span・イテレータには無効化条件を記載します。
- プール配置などの性能変更は代表入力で時間・メモリを比較します。整数ID方式ではvector追加中の
  参照無効化、ポインタ方式では格納領域のアドレス安定性を確認してください。

## 平衡二分木の拡張規約

- 平衡化、ノードの集約・遅延作用、列/LCTの操作、ストレージの責務を分離します。
  回転はBinaryTreeOps、平衡化はSplayBase/TreapBase、列操作はSequenceBase、動的森はLinkCutBaseを使います。
- 共通コアはノードを所有せず、NodePoolや固定配列への依存を持ちません。公開ラッパーが
  ノード・ルートを隠し、従来のコピー禁止・move・clone・消費操作の規約を維持します。
- 新しい平衡化方式は `prepare`、`access(root&,k)`、`split`、`join`、`build(n,factory)` の
  Backend契約を実装し、列操作を複製しません。契約の詳細は
  [SequenceBase reference](docs/reference/data_structure/internal/sequence_base.md) に従ってください。
- 集約はPolicyのpush/pull/toggle/applyに委譲します。composition(f,g)はf∘gです。
  非可換演算では順逆集約を維持し、反転と作用の整合条件をreferenceに明記します。
- ノードの高さ・優先度などはPolicyのExtraに保持します。通常版へダミーの遅延タグを追加したり、
  全ノードへ未使用の頂点番号・サイズを追加したりしないでください。virtual関数・型消去は用いません。
- LCTのppは補助木の親またはpath-parentです。根判定は親の子リンクとの対応で行い、
  exposeで外した右の子のppを無条件に消しません。回転では関連タグをpush済みにし、構成が変わる
  2ノードを下からpullします。path-parentを通常の祖先としてpullしないでください。
- 通常の列は1ノード1要素、DynamicBitVectorは1ノード複数bitです。重み付き探索で返すoffsetと、
  ノード間のsplit境界を区別します。ブロック内部の分割はpayload側が担当します。
- 構築のO(N)を維持してください。Cartesian tree構築はbuild_cartesianを共有し、
  親子配列が不要な用途へ中間配列を強制しません。疑似乱数のseedを固定できる内部APIを残します。
- 検証では同じ列テストを異なるBackendで実行し、非可換集約、反転と作用の混在、LCTの
  preferred edge変更、bit列の最終部分ブロック・rank(n)・削除後の再利用を確認します。
  変更前のワークツリーと時間・RSS・ノードサイズを比較してください。

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
  make verify TARGETS=unit/<name>
  ```

- 現行 `tools/verify.py` は `--offline` / `--no-generate` に対応していません。
  利用可能な引数は `python3 tools/verify.py --help` で確認してください。

- 変更後は少なくとも `make check` と `git diff --check` を実行してください。
  `make check` は先頭URL、依存切れ、循環依存に加えreferenceの網羅性・必須節・リンクを検査します。
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
  `docs/reference/**/*.md`、`README.md`、サンプルも更新してください。
- 新規ヘッダーには同じ相対パスのMarkdown referenceを追加し、
  [reference索引](docs/reference/README.md) に登録してください。
  既存の `*_document.txt` はMarkdownへ移行済みです。新しいtxt版は作らないでください。
- referenceには機能・前提条件、インターフェース、計算量、依存関係、Verify・利用例を記載します。
  半開/閉区間、添字、構築順、戻り値・番兵、型制約、可換性・作用合成順を必要に応じて説明します。
  計算量の記号と、期待値・償却・最悪上界の区別を明示してください。
- 宣言の抜粋だけでなく意味や制約も更新し、実装本体のコピーをreferenceに置かないでください。
  本文は手で管理します。構造だけの確認は `python3 tools/check_references.py` で実行できます。
- デフォルトテンプレート引数を追加・変更する場合は、従来の省略形がコンパイルできることと、
  明示指定した形の両方をテストしてください。
- `fastio.hpp` と通常のiostreamの双方から利用される入出力演算子では、独自パーサを重複実装せず、
  既存の整数入出力との互換性を優先してください。

## 完了時の報告

- 変更した主要ファイルと公開APIへの影響を簡潔にまとめてください。
- 実行したユニットテスト、Library Checkerターゲット、`make check` の結果を報告してください。
- ベンチマークを行った場合は、入力、回数、変更前後の値、採用判断を記載してください。
- テスト未実行、既存不具合、環境上の制約があれば隠さず明記してください。
