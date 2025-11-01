#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// コマンドラインオプションのカテゴリを定義
static cl::OptionCategory AddBracesCategory("add-braces options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// 波括弧を追加するASTVisitor
class AddBracesVisitor : public RecursiveASTVisitor<AddBracesVisitor> {
public:
  explicit AddBracesVisitor(Rewriter &R) : TheRewriter(R) {}

  // if文を訪問
  bool VisitIfStmt(IfStmt *S) {
    // then節をチェック
    Stmt *Then = S->getThen();
    if (Then && !isa<CompoundStmt>(Then)) {
      addBraces(Then);
    }

    // else節をチェック
    Stmt *Else = S->getElse();
    if (Else && !isa<CompoundStmt>(Else) && !isa<IfStmt>(Else)) {
      // else if の場合は波括弧を追加しない
      addBraces(Else);
    }

    return true;
  }

  // while文を訪問
  bool VisitWhileStmt(WhileStmt *S) {
    Stmt *Body = S->getBody();
    if (Body && !isa<CompoundStmt>(Body)) {
      addBraces(Body);
    }
    return true;
  }

  // for文を訪問
  bool VisitForStmt(ForStmt *S) {
    Stmt *Body = S->getBody();
    if (Body && !isa<CompoundStmt>(Body)) {
      addBraces(Body);
    }
    return true;
  }

  // do-while文を訪問
  bool VisitDoStmt(DoStmt *S) {
    Stmt *Body = S->getBody();
    if (Body && !isa<CompoundStmt>(Body)) {
      addBraces(Body);
    }
    return true;
  }

  // range-based for文を訪問
  bool VisitCXXForRangeStmt(CXXForRangeStmt *S) {
    Stmt *Body = S->getBody();
    if (Body && !isa<CompoundStmt>(Body)) {
      addBraces(Body);
    }
    return true;
  }

private:
  Rewriter &TheRewriter;

  // 文の前後に波括弧を追加する
  void addBraces(Stmt *S) {
    SourceManager &SM = TheRewriter.getSourceMgr();
    const LangOptions &LangOpts = TheRewriter.getLangOpts();

    // 文の開始位置を取得
    SourceLocation StartLoc = S->getBeginLoc();

    // 文の終了位置を取得
    SourceLocation EndLoc = S->getEndLoc();

    // トークンの終わりまで移動（セミコロンの後など）
    EndLoc = Lexer::getLocForEndOfToken(EndLoc, 0, SM, LangOpts);

    if (StartLoc.isInvalid() || EndLoc.isInvalid()) {
      return;
    }

    // マクロ展開内の場合はスキップ
    if (StartLoc.isMacroID() || EndLoc.isMacroID()) {
      return;
    }

    // インデントを取得
    std::string Indent = getIndentation(StartLoc, SM);

    // 開き波括弧を追加（改行付き）
    TheRewriter.InsertTextBefore(StartLoc, "{\n" + Indent);

    // 閉じ波括弧を追加（改行付き）
    TheRewriter.InsertTextAfterToken(EndLoc, "\n" + Indent + "}");
  }

  // インデントを取得する補助関数
  std::string getIndentation(SourceLocation Loc, SourceManager &SM) {
    // 行の開始位置を取得
    SourceLocation LineStart = SM.translateLineCol(
        SM.getFileID(Loc), SM.getSpellingLineNumber(Loc), 1);

    if (LineStart.isInvalid()) {
      return "    "; // デフォルトのインデント
    }

    // 行の開始から現在位置までのテキストを取得
    const char *LineStartPtr = SM.getCharacterData(LineStart);
    const char *LocPtr = SM.getCharacterData(Loc);

    std::string LinePrefix(LineStartPtr, LocPtr - LineStartPtr);

    // 空白文字のみを抽出
    std::string Indent;
    for (char c : LinePrefix) {
      if (c == ' ' || c == '\t') {
        Indent += c;
      } else {
        break;
      }
    }

    // 追加のインデントを加える
    if (Indent.find('\t') != std::string::npos) {
      Indent += '\t';
    } else {
      Indent += "    "; // 4スペース
    }

    return Indent;
  }
};

// ASTConsumerの実装
class AddBracesConsumer : public ASTConsumer {
public:
  explicit AddBracesConsumer(Rewriter &R) : Visitor(R) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  AddBracesVisitor Visitor;
};

// FrontendActionの実装
class AddBracesAction : public ASTFrontendAction {
public:
  AddBracesAction() = default;

  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();

    // 変更をファイルに書き込む
    const RewriteBuffer *RewriteBuf = TheRewriter.getRewriteBufferFor(SM.getMainFileID());
    if (RewriteBuf) {
      llvm::outs() << "// Modified file:\n";
      llvm::outs() << std::string(RewriteBuf->begin(), RewriteBuf->end());
    } else {
      llvm::outs() << "// No changes made\n";
    }
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef File) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<AddBracesConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, AddBracesCategory);
  if (!ExpectedParser) {
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser &OptionsParser = ExpectedParser.get();

  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  return Tool.run(newFrontendActionFactory<AddBracesAction>().get());
}
