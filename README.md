# Add Braces Tool - C/C++ コード自動整形ツール

descriptionの意味は、「if分の後に波括弧を書かないエンジニアはドアの角に足の指ぶつけろ」です。

このツールは、C/C++のコードをASTレベルで解析し、if/else/while/forなどの制御文の後に波括弧 `{}` がない場合に自動的に追加します。

## 機能

- **if文**: then節とelse節の両方をサポート
- **while文**: ループ本体に波括弧を追加
- **for文**: 通常のforとrange-based forの両方に対応
- **do-while文**: ループ本体に波括弧を追加
- **ネスト対応**: ネストされた制御文も正しく処理
- **else if対応**: else ifチェーンを壊さないように処理

## 必要な環境

- CMake 3.13以上
- LLVM/Clang 10以上（開発用のヘッダーとライブラリが必要）
- C++17対応のコンパイラ

### Ubuntu/Debianでのインストール

```bash
sudo apt-get update
sudo apt-get install llvm-dev libclang-dev clang cmake build-essential
```

### macOSでのインストール

```bash
brew install llvm cmake
```

LLVMがHomebrewでインストールされた場合、以下の環境変数を設定してください：

```bash
export LLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm
export Clang_DIR=/opt/homebrew/opt/llvm/lib/cmake/clang
```

## ビルド方法

```bash
# プロジェクトディレクトリに移動
cd add_braces

# ビルドディレクトリを作成
mkdir build
cd build

# CMakeで設定
cmake ..

# ビルド
make

# 実行ファイルが build/add-braces として生成されます
```

## 使い方

### 基本的な使用方法

```bash
./add-braces <ソースファイル> -- [コンパイルオプション]
```

### 例

```bash
# シンプルな実行
./add-braces ../test_sample.cpp --

# C++17を指定
./add-braces ../test_sample.cpp -- -std=c++17

# インクルードパスを指定
./add-braces ../test_sample.cpp -- -I/path/to/includes -std=c++17
```

### 出力

デフォルトでは、変更されたコードが標準出力に表示されます。

```bash
# ファイルに保存する場合
./add-braces test_sample.cpp -- > test_sample_fixed.cpp

# 元のファイルを上書きする場合（注意：バックアップを取ってから実行）
./add-braces test_sample.cpp -- > test_sample_tmp.cpp && mv test_sample_tmp.cpp test_sample.cpp
```

## 変換例

### Before (変換前)

```cpp
if (x > 5)
    std::cout << "x is greater than 5" << std::endl;

if (x < 20)
    std::cout << "x is less than 20" << std::endl;
else
    std::cout << "x is 20 or greater" << std::endl;

while (i < 3)
    std::cout << "i = " << i++ << std::endl;

for (int j = 0; j < 3; j++)
    std::cout << "j = " << j << std::endl;
```

### After (変換後)

```cpp
if (x > 5) {
    std::cout << "x is greater than 5" << std::endl;
}

if (x < 20) {
    std::cout << "x is less than 20" << std::endl;
}
else {
    std::cout << "x is 20 or greater" << std::endl;
}

while (i < 3) {
    std::cout << "i = " << i++ << std::endl;
}

for (int j = 0; j < 3; j++) {
    std::cout << "j = " << j << std::endl;
}
```

## 実装の詳細

このツールは以下のClang libtoolingコンポーネントを使用しています：

- **RecursiveASTVisitor**: ASTを再帰的に訪問
- **Rewriter**: ソースコードの変更を管理
- **IfStmt, WhileStmt, ForStmt, DoStmt, CXXForRangeStmt**: 各種制御文のASTノード

### 主要なクラス

- `AddBracesVisitor`: ASTを訪問して波括弧が必要な箇所を検出
- `AddBracesConsumer`: ASTConsumerの実装
- `AddBracesAction`: FrontendActionの実装

## 制限事項

- マクロ展開内のコードは変更されません
- コメントの位置によっては、フォーマットが期待と異なる場合があります
- インデントは自動検出されますが、混在したスタイルには対応していない場合があります

## トラブルシューティング

### "LLVM not found"エラー

LLVMのCMake設定ファイルが見つからない場合、以下のように明示的にパスを指定してください：

```bash
cmake -DLLVM_DIR=/usr/lib/llvm-14/cmake ..
```

### リンクエラー

必要なClangライブラリがインストールされているか確認してください：

```bash
# Ubuntuの場合
sudo apt-get install libclang-cpp-dev
```

## ライセンス

このツールはMITライセンスの下で提供されています。

## 貢献

バグ報告や機能リクエストは、Issueとして報告してください。
