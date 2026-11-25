# アーキテクチャと開発ガイド

ライブラリの内部設計と開発時の確認手順をまとめます。利用方法は [README](README.md)、
作業時の具体的な規約は [AGENTS.md](AGENTS.md)、各APIの契約は
[reference](docs/reference/README.md) を参照してください。

## 構成とinclude

C++20のヘッダーオンリーライブラリです。`library/` を分野別に分け、各ヘッダーに対応する
説明を `docs/reference/`、検証コードを `verify/` に置きます。`third_party/` は外部コードです。

ライブラリ間のincludeは、読み込む側のヘッダーのディレクトリを基準とした相対パスに統一します。

```cpp
// library/math/modint.hpp から数学の内部実装を参照
#include "internal/internal_math.hpp"
// library/math/discrete_logarithm.hpp から別分野を参照
#include "../data_structure/hash_table.hpp"
// library/data_structure/splay_tree.hpp から内部実装を参照
#include "internal/splay_base.hpp"
```

標準ライブラリやAtCoder Libraryは従来どおり `<...>` で読み込みます。
解答コード・verify・referenceの利用例は `#include "library/..."` の形式で公開ヘッダーを指定できます。
各ヘッダーは必要な標準ヘッダーと直接依存する自作ヘッダーを明示し、推移的includeに依存しません。
先頭のGitHub URLコメントはリポジトリルートからのパスを維持します。

`tools/library_manager.py` は相対includeを正規化して `library/...` の依存グラフに変換し、
依存切れ・循環依存・ヘッダー内のルート基準includeを検査します。
解答コードの展開は `tools/expand_include.py` が担当します。どちらも同じファイルの重複展開を避けます。

数学の公開ヘッダーは `library/math/` に置き、共通の内部実装と型traitsは `library/math/internal/` に置きます。
`bit_width`、`countr_zero`、`popcount` などを使う実装は `internal_bit.hpp` を直接includeし、
`soryuusi::` で修飾して呼びます。標準の `<bit>`、処理系の組み込み関数、代替実装の切り替えは、
このヘッダーが担当します。ビット数を調べる引数は対応する符号なし整数型にそろえます。

宣言、関数・テンプレートの引数列、単一の式は1行にまとめます。関数本体の文や、複数要素を持つ
初期化の区切りには改行を使います。

## ポインタ木と永続構造

可変構造は原則コピー禁止・move可能とし、独立複製には `clone()` を使います。
ノード型・ルートポインタ・内部配列は隠し、添字・頂点番号・列ハンドルで操作します。
安定アドレスのブロック型プール、または再確保に安全な整数IDを使います。
ノードごとの参照カウントやdeleteは要求せず、領域単位での解放を基本とします。
不要ノードの保持も許容します。公開済み永続ノードは再利用しません。

永続構造は管理オブジェクトと不透明な値型 `Root` を分離します。
更新は `[[nodiscard]]` な新しいRootを返し、原則Rootを先頭の引数に取ります。
検索はconstで、既存版を変更せずノードも確保しません。版番号は利用側のvectorなどで管理します。
Rootは借用ハンドルで、別管理領域の版を混ぜる操作はデバッグ時に検査します。
管理領域の破棄・move代入による置換で無効になりますが、move構築では移動先に引き継ぎます。
永続DSUのinitは一度だけで、`empty_root()` は全頂点が独立した初期版です。

split/concatやmerge/meldは消費元を空にします。同一arenaでは所有領域だけを共有し、
可変部分木を共有しません。別arena間では値を移送します。
LCTは複数の木を1つの管理オブジェクトに置き、固定配列の頂点番号で操作します。
trieのmergeは集合和、Li Chao木のmergeは包絡の和です。

型名は通常版・遅延版・永続版が同時にincludeできる正規名を使い、旧型名の互換エイリアスは提供しません。
既存の一意な関数名・引数順には互換ラッパーを残します。
新しい永続APIはRootを先頭に渡し、区間コピーは `copy_range(destination, source, l, r)` とします。

## 平衡二分木の責務と拡張

平衡化、集約・作用、列/LCTの操作、所有権を分離します。
splay/treapの管理型はarenaを確保し、`new_sequence()` でmove-onlyの列ハンドルを作ります。
明示キー版は `new_set()` で集合を作ります。ハンドルはarenaを共有所有し、管理型の破棄後も使用できます。
cloneも同じarenaに独立したノードを作ります。異arenaへの複製は値を取り出して構築します。

従来の明示キー版はキーを固定し、集約と遅延作用を別に持つ値に適用します。
Monotone版はExtraにキー作用を保持し、減少方向の更新を部分木反転と順逆集約の交換で扱います。
LazyMonotone版はキー作用と値作用のタグを独立に持ち、既存のPolicyを組み合わせます。
更新後の区間を元の境界へ結合できない場合はmeldで統合します。
重複可否はテンプレート引数Multiで選びます。meldは同値キーでは連結先の要素を先に置き、
重複不可では連結先の値を残します。順位区間とキー区間のAPIを区別します。

開発時の分担は次のとおりです。

- [BinaryTreeOps](docs/reference/data_structure/internal/binary_tree_ops.md)：非所有の回転・重み付き位置探索。
- [ReversibleMonoid](docs/reference/data_structure/internal/reversible_monoid.md)：順逆集約、反転、任意の遅延作用、必要なノード状態。
- [SplayBase](docs/reference/data_structure/internal/splay_base.md) / [TreapBase](docs/reference/data_structure/internal/treap_base.md)：平衡化。ノードの確保・解放は行いません。
- [TreeArena](docs/reference/data_structure/internal/tree_arena.md)：プール、再利用リスト、Backendの作業領域と、取得された要素参照を管理。
- [SequenceBase / SequenceArena](docs/reference/data_structure/internal/sequence_base.md)：列の生成と操作。新しい平衡化方式も同じBackendインターフェースで接続できます。
- [ExplicitSet / ExplicitArena](docs/reference/data_structure/internal/explicit_base.md)：明示キー集合と区間演算・単調キー作用・meld。
- [SortableSequence / SortableArena](docs/reference/data_structure/internal/sortable_base.md)：Explicit木を区間とする重み付きSequence。既存のBackendで外側の列を分割・結合し、ソート時は対象区間の内側だけをmeld。
- [LinkCutBase](docs/reference/data_structure/internal/link_cut_base.md)：preferred edgeとpath-parentを管理するLCT操作。固定配列を所有します。

追加の平衡化方式には、`prepare`、`access(root&,k)`、`split`、`join`、`build` を実装します。
探索済み経路を使うアクセス、単要素の挿入・削除、区間への直接アクセスはBackendで提供できます。
Sortableの区間集約では区間構成を維持し、完全に含まれる部分木の保存済み集約を使います。
実装と計測条件は [平衡二分木の性能比較](docs/reference/data_structure/internal/sequence_base.md#区間処理meldの高速化) を参照してください。
ノードの追加情報はPolicyのExtraに保持でき、既存の列操作を複製する必要はありません。
LCTの補助木にはpath-parentを維持するsplay相当の契約が必要です。

通常版には遅延タグを、LCTには不要な列サイズを持たせません。bit列では1ノードが複数bitを持つため、
ノード数と論理要素数を区別します。共通コアは静的なテンプレート呼出しを使い、virtual関数や型消去を使いません。
Cartesian treeの線形構築も、整数ID用とtreap用で共有しています。

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

## 変更時の検証

まず対象のreference、verify、利用箇所を確認します。変更した機能に直接・推移的に依存する
ターゲットから実行し、APIの契約もreferenceへ反映します。

```sh
make affected BASE=origin/main
make verify TARGETS=unit/balanced_trees
make check
git diff --check
python3 -m unittest discover -s tools -p 'test_*.py'
```

include展開へ影響する変更では、展開したコードも検証します。

```sh
make expand-verify TARGETS=unit/balanced_trees
make verify TARGETS=unit/balanced_trees VERIFY_ARGS=--expanded
```

ポインタ木では版の分岐、検索時の確保数不変、move/clone、消費操作後の独立性を確認します。
平衡木では非可換集約、反転と作用、複数ハンドル、管理型破棄後の操作、重複とmeldも検証します。
性能やメモリ配置を変える場合は代表入力で時間・RSS・ノードサイズを比較します。

オンラインジャッジのターゲットの追加方法は
[Library Checker](verify/library_checker/README.md) / [AOJ](verify/aoj/README.md)、
ツールとCIの詳細は [tools/README.md](tools/README.md) にあります。
READMEには利用方法を記載し、内部設計や開発手順はこの文書とAGENTS.mdで管理します。

## 検証結果の管理

Git差分と成功実績を分けます。差分は変更説明・候補列挙に使い、実行省略は現在の入力と
成功記録の検証キーが一致する場合だけ認めます。CIは全ターゲットのキーを照合するため、
新規ブランチ・rebase・PR merge SHA・差分取得不能でも成功実績を利用できます。

責務は`verify_cache.py`が依存解析・キー・成功記録、`verify.py`が計画と実行、
`verify_online.py`がオンライン実行の完了確認、`verify_sync.py`が独立したGitブランチとの同期です。
通常版と展開版は別の検証条件です。unitの全構成が成功することを要求し、オンラインはhelperの
timestampを使わないmarkerで全ケースの完了を確認します。実行前後に入力を照合します。

キーにはcompilerの実際のinclude依存、manifest、実行処理の内容、コンパイラ・標準ライブラリ・
OS・Python・オンラインツールの版、オプション、ケースの版、実行条件を含めます。
展開器は生成結果を介して反映します。文書・レポート・同期処理の変更だけでは無効化しません。
ケースは`verify/cases.json`の固定版・epochと有効期間で管理し、取得後の内容も記録します。
ケース取得や実行が途中で終わったときに、成功記録を作らないことが重要です。

共有ブランチ`verify-results`には検証キーを名前とするJSONだけを保存します。
強制再実行の失敗・中断は古い成功の無効化記録として保持し、取得・統合で過去の成功が復活しないようにします。
共有時は一時リポジトリを使い、競合時は再取得・集合の統合を行います。既存結果をforce-pushで置換しません。
CIのmatrix jobはartifactへ成功結果を保存し、集約jobが途中失敗分を含めて完了した成功結果だけを公開します。
fork由来のPRからは公開しません。ローカルの共有も明示的な操作とし、同じ環境の成功記録だけを再利用します。
環境を揃えるためCIのUbuntuコンテナと`tools/verify.Dockerfile`は同じセットアップスクリプトを使います。

運用コマンド、期限切れと強制実行、ケース更新方法は[tools/README.md](tools/README.md#成功記録と再実行)を参照してください。
