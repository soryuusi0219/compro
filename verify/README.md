# Verification

自作ライブラリをオンラインジャッジのテストケースで検証するコードを置きます。

現在は `yosupo06/library-checker-problems` を利用します。詳細は
[library_checker/README.md](library_checker/README.md) を参照してください。

```text
verify/
└── library_checker/
    ├── config.json
    ├── manifests/             # verifyソースと同じ分野別構造の検証設定
    │   ├── data_structure/
    │   ├── graph/
    │   └── ...
    ├── data_structure/
    ├── graph/
    └── ...
```

各検証コードは `library/` の実装を読み込み、問題固有の入出力と最小限の接続処理だけを持たせます。
