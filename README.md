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

### ライブラリのreference

全ヘッダーの機能、前提条件、インターフェース、計算量、依存関係、verifyへのリンクを
[`docs/reference/README.md`](docs/reference/README.md) にまとめています。
各ページのパスはヘッダーと対応します。例えば `library/math/matrix.hpp` のreferenceは
[`docs/reference/math/matrix.md`](docs/reference/math/matrix.md) です。
既存のtxtドキュメントも対応するMarkdownへ統合しています。

開発時の作業規約は [`AGENTS.md`](AGENTS.md)、referenceの読み方・更新方法は
[reference索引](docs/reference/README.md#読み方) を参照してください。
`make check` はreferenceの網羅性、必須項目、ローカルリンクも検査します。

## ポインタ木・永続構造の所有権

可変のポインタ木は原則コピー禁止・move可能です。独立した複製が必要な場合は
`clone()` を使います。ノード・ルートポインタ・内部配列は公開せず、添字や列の操作で扱います。
ノードの確保にはアドレスが変わらないプールを使い、個別のdeleteや参照カウントを避けます。
不要になったノードを管理領域の終了まで保持する実装も許容します。

永続構造は管理オブジェクトと不透明な値型 `Root` を分けます。Rootのコピーは安価で、
版番号は利用側の `std::vector<Root>` などで管理します。更新は `[[nodiscard]]` な新しいRootを返し、
検索は入力版を変更せずノードも確保しません。公開済みノードへの破壊的操作は提供しません。

```cpp
soryuusi::Persistent_Vector<int> array;
using Root = decltype(array)::Root;
std::vector<Root> versions{array.empty_root()};
versions.push_back(array.set(versions[0], 3, 10));
versions.push_back(array.set(versions[0], 3, 20)); // 初期版から別に分岐
int x = array.get(versions[1], 3);                // 10
```

Rootは借用ハンドルです。管理領域の破棄・move代入による置換で無効になります。
管理オブジェクトをmove構築した場合は移動先で引き続き使用できます。
同じ型でも別の管理領域の版は混ぜられず、デバッグ時に所有元を検査します。
永続DSUの `empty_root()` は全頂点が独立した初期版です。DSUのinitは1回だけ呼びます。

splayの `split(k)` は呼出元を空にして2つの列を返し、`concat(std::move(other))` は
相手を空にして末尾へ連結します。同一プール内の連結は償却対数時間、別プール間は
相手の値を移してから接続します。split後の列はプールの所有権だけを共有し、可変ノードは共有しません。
LCTでは複数の木を1つの管理オブジェクト内に置き、頂点番号でlink/cutします。
trieのmergeは集合和、Li Chao木のmergeは包絡の和で、どちらも相手を消費します。

従来の型名衝突を解消するため、以下の名前を使用します。

| 対象 | 型名・移行方法 |
| --- | --- |
| 永続動的セグ木 | `PersistentDynamicSegtree<S,op,e>` |
| 通常splay | `SplayTree<T,op,e>` |
| 遅延splay | `LazySplayTree<T,op,e,F,mapping,composition,id>` |
| 永続DSU | `PersistentDSU` |
| データ付き永続DSU | `PersistentDSUWithData<S,op>` |
| 版の型 | 各構造の `Root` |

生ノード操作と永続配列の破壊的APIは廃止しました。旧型名の互換エイリアスは提供しません。
既存の一意な関数名・引数順は互換ラッパーを残し、新APIは原則としてRootを先頭に渡します。
区間コピーは `copy_range(destination, source, l, r)` とします。
詳細と計算量は各[reference](docs/reference/README.md)を参照してください。

## 平衡二分木の共通基盤

splay・treap・LCTは、平衡化、集約・作用、操作、所有権を分離しています。
公開する列型は同じ操作を持ち、平衡化方式だけを選べます。

| 型 | 平衡化と用途 |
| --- | --- |
| `SplayTree` / `LazySplayTree` | 列操作は償却対数時間 |
| `TreapSequence` / `LazyTreapSequence` | 同じ列操作を、優先度による期待対数時間で提供 |
| `linkcuttree` / `lazy_linkcuttree` | splayを補助木に用いる動的森 |
| `SplayExplicit` / `LazySplayExplicit` | 固定キー順の集合、順位・キー区間演算、meld |
| `TreapExplicit` / `LazyTreapExplicit` | 同じ明示キー操作をtreapで提供 |
| `DynamicBitVector` | bit圧縮したブロックを重み付きtreapで管理 |
| `DynamicWavelet` | 独立したDynamicBitVectorを各段で使用 |

splay・treapの管理型はarenaだけを確保し、列はnew_sequence()で作ります。列の型はManager::Sequenceです。
列ハンドルはmove-onlyで、既定構築できません。管理型のmove・破棄後も、共有所有するarenaはハンドルが保持します。
cloneは同じarenaに独立したノードを作り、異arenaへの複製は別管理型のnew_sequence(sequence.to_vector())で行います。
LCTの呼出し形式は維持しています。
```cpp
soryuusi::SplayTree<long long, op, e> arena;
auto a = arena.new_sequence({1, 2});
auto b = arena.new_sequence({3, 4});
a.concat(std::move(b)); // 同じarena内で接続、bは空
```

新しい列型の使用方法は [treap reference](docs/reference/data_structure/treap.md)、
bit列の操作は [DynamicBitVector reference](docs/reference/data_structure/dynamic_bit_vector.md) を参照してください。

明示キー版は管理型から `new_set()` で集合を作ります。テンプレート引数Multiでキーの重複可否を選び、
集約と遅延作用はキーとは別の値へ適用します。`prod(l,r)` / `apply(l,r,f)` は順位区間、
`prod_key(low,high)` / `apply_key(low,high,f)` はキーの半開区間です。
`meld(std::move(other))` は相手を消費し、同値キーでは連結先の要素を先に置きます。
重複不可では連結先の値を残します。別arena間でも移送を伴うmeldを使用できます。
詳細は [splay_explicit](docs/reference/data_structure/splay_explicit.md) / [treap_explicit](docs/reference/data_structure/treap_explicit.md) を参照してください。

開発時の分担は次のとおりです。

- [BinaryTreeOps](docs/reference/data_structure/internal/binary_tree_ops.md)：非所有の回転・重み付き位置探索。
- [ReversibleMonoid](docs/reference/data_structure/internal/reversible_monoid.md)：順逆集約、反転、任意の遅延作用、必要なノード状態。
- [SplayBase](docs/reference/data_structure/internal/splay_base.md) / [TreapBase](docs/reference/data_structure/internal/treap_base.md)：平衡化。ノードの確保・解放は行いません。
- [TreeArena](docs/reference/data_structure/internal/tree_arena.md)：プール、再利用リスト、Backendの作業領域を共有。
- [SequenceBase / SequenceArena](docs/reference/data_structure/internal/sequence_base.md)：列の生成と操作。新しい平衡化方式も同じBackendインターフェースで接続できます。
- [ExplicitSet / ExplicitArena](docs/reference/data_structure/internal/explicit_base.md)：固定キーの集合と区間演算・meld。
- [LinkCutBase](docs/reference/data_structure/internal/link_cut_base.md)：preferred edgeとpath-parentを管理するLCT操作。固定配列を所有します。

追加の平衡化方式には、`prepare`、`access(root&,k)`、`split`、`join`、`build` を実装します。
ノードの追加情報はPolicyのExtraに保持でき、既存の列操作を複製する必要はありません。
LCTの補助木にはpath-parentを維持するsplay相当の契約が必要です。

通常版には遅延タグを、LCTには不要な列サイズを持たせません。bit列では1ノードが複数bitを持つため、
ノード数と論理要素数を区別します。共通コアは静的なテンプレート呼出しを使い、virtual関数や型消去を使いません。
Cartesian treeの線形構築も、整数ID用とtreap用で共有しています。

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
