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
make verify-list
make verify TARGETS=library_checker/convolution_mod
make expand path/to/main.cpp
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

ライブラリ間の依存はヘッダーからの相対パスで読み込むため、`library/` を別の場所へ
コピーして利用することもできます。ライブラリのファイル名は、小文字の `snake_case` を基本としています。

### ライブラリのreference

全ヘッダーの機能、前提条件、インターフェース、計算量、依存関係、verifyへのリンクを
[`docs/reference/README.md`](docs/reference/README.md) にまとめています。
各ページのパスはヘッダーと対応します。例えば `library/math/matrix.hpp` のreferenceは
[`docs/reference/math/matrix.md`](docs/reference/math/matrix.md) です。

## 木構造・永続構造の使い方

可変の木構造は原則コピー禁止・move可能です。独立した複製には `clone()` を使います。
splay・treapでは、管理型を宣言してから `new_sequence()` で列を作ります。

```cpp
soryuusi::SplayTree<long long, op, e> arena;
auto a = arena.new_sequence({1, 2});
auto b = arena.new_sequence({3, 4});
a.concat(std::move(b)); // aは{1,2,3,4}、bは空
```

列の型は `Manager::Sequence` で、既定構築・コピーはできません。
`split(k)` は呼出元を空にして2つの列を返します。列ハンドルは管理型の破棄後も使えます。
通常版・遅延版の操作は [splay](docs/reference/data_structure/splay_tree.md) /
[treap](docs/reference/data_structure/treap.md) を参照してください。
列と集合では `ref_at(i)` で要素参照を取り、分割・結合後も `index_of(ref)` で現在の順位を調べられます。
その列・集合に要素がなければ `-1` を返します。

キー付きの集合は `SplayExplicit` / `TreapExplicit` の `new_set()` で作ります。
テンプレート引数Multiで重複可否を選べます。`prod(l,r)` / `apply(l,r,f)` は順位区間、
`prod_key(low,high)` / `apply_key(low,high,f)` はキーの半開区間を扱い、作用は値だけに適用します。
キーを単調に更新する場合は `MonotoneSplayExplicit` / `MonotoneTreapExplicit` の
`apply_keys(l,r,f)` を使います。単調減少の作用では第4引数を `true` にします。
キーと値の両方に遅延作用を使う場合は `LazyMonotoneSplayExplicit` / `LazyMonotoneTreapExplicit` を選びます。
`meld(std::move(other))` は相手を空にし、同値キーでは連結先の要素を先に置きます。
重複不可の場合は連結先の値を残します。詳細は
[splay_explicit](docs/reference/data_structure/splay_explicit.md) /
[treap_explicit](docs/reference/data_structure/treap_explicit.md) を参照してください。

列の順序を普段は保ち、指定した区間だけキー順に並べ替える場合は `SplaySortable` /
`TreapSortable` を使います。`new_sequence({{key, value}, ...})` で作り、
`sort_inc(l,r)` / `sort_dec(l,r)` で並べた後の値を `prod(l,r)` で集約します。
値・キーへの遅延作用も選べます。 [splay_sortable](docs/reference/data_structure/splay_sortable.md) /
[treap_sortable](docs/reference/data_structure/treap_sortable.md)

meldと全キー加算が必要な最小ヒープには `SkewHeap` を使います。`new_heap()` で作った同じarenaの
ヒープ同士をmeldできます。版を分岐させる場合は `PersistentSkewHeap` を使います。
[skew_heap](docs/reference/data_structure/skew_heap.md)

永続構造では更新が返す `Root` を保存して版を管理します。

```cpp
soryuusi::Persistent_Vector<int> array;
using Root = decltype(array)::Root;
std::vector<Root> versions{array.empty_root()};
versions.push_back(array.set(versions[0], 3, 10));
versions.push_back(array.set(versions[0], 3, 20)); // 初期版から分岐
int x = array.get(versions[1], 3);                // 10
```

Rootは作成した管理オブジェクトの有効期間内で使い、異なる管理領域のRootを混ぜないでください。
管理オブジェクトのmove構築後は移動先で使えますが、move代入で置換された領域のRootは無効です。
永続DSUのinitは1回だけ呼び、`empty_root()` を全頂点が独立した初期版として使います。
詳細は [永続配列](docs/reference/data_structure/persistent_vector.md) など各referenceを参照してください。

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
```

ターゲット名はそれぞれ `library_checker/<name>`、`aoj/<name>`、`unit/<name>` です。
同じソース・依存関係・実行条件で成功済みのターゲットは省略します。
展開した提出コードの検証、実行予定の確認、強制再実行は次のように指定できます。

```sh
make verify VERIFY_ARGS=--expanded
make verify-plan
make verify TARGETS=unit/rolling_hash VERIFY_ARGS=--force
```

CIなどと成功記録を共有する場合は、検証前に取得し、検証後に公開します。
CIの結果を再利用する場合は、同じ環境で`--expanded`を指定します。

```sh
make verify-sync
make verify TARGETS=unit/rolling_hash VERIFY_ARGS=--expanded
make verify-sync VERIFY_ARGS='--sync push'
```

ツールごとの詳しい引数は [`tools/README.md`](tools/README.md) にまとめています。

内部構成・開発手順は [architecture.md](architecture.md) にまとめています。

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
