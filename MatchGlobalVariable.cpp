#include<iostream>
#include<vector>
using namespace std;

// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

/****************************************************************************/

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;


DeclarationMatcher GlobalVarMatcher = varDecl(
	hasGlobalStorage(), unless(hasAncestor(functionDecl()))
).bind("gvar");

class GlobalVariablePrinter : public MatchFinder::MatchCallback {
public:
	virtual void run(const MatchFinder::MatchResult &Result) {
		if (const VarDecl * VD = Result.Nodes.getNodeAs<VarDecl>("gvar")) {
			VD->dump();
			cout << VD->getQualifiedNameAsString() << ": ";
			cout << VD->getType().getAsString() << endl;
		}
	}
};

DeclarationMatcher UsedGlobalVarFunction = functionDecl(
	hasDescendant(declRefExpr(
		to(
			varDecl(
				hasGlobalStorage(), unless(hasAncestor(functionDecl()))
			)
		)
	))
).bind("gfun");

class UsedGlobalVarFunctionPrinter : public MatchFinder::MatchCallback {
public:
	virtual void run(const MatchFinder::MatchResult &Result) {
		if (const FunctionDecl * FD = Result.Nodes.getNodeAs<FunctionDecl>("gfun")) {
			FD->dump();
			cout << FD->getNameAsString() << ": ";
			cout << FD->getCallResultType().getAsString();
			int paraNum = FD->getNumParams();
			printf("(");
			for (int i = 0; i < paraNum; i++) {
				if (i)printf(", ");
				cout << FD->getParamDecl(i)->getType().getAsString() << " ";
				cout << FD->getParamDecl(i)->getNameAsString();
			}
			printf(")\n");
		}
	}
};

int main(int argc, const char **argv) {
	CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
	ClangTool Tool(OptionsParser.getCompilations(),
		OptionsParser.getSourcePathList());

	GlobalVariablePrinter GvarPrinter;
	UsedGlobalVarFunctionPrinter GfunPrinter;
	MatchFinder GlobalVariableFinder;
	GlobalVariableFinder.addMatcher(GlobalVarMatcher, &GvarPrinter);

	freopen("F://Desktop/GlobalVariable.txt", "w", stdout);
	Tool.run(newFrontendActionFactory(&GlobalVariableFinder).get());
	GlobalVariableFinder.~MatchFinder();

	MatchFinder FunctionFinder;
	FunctionFinder.addMatcher(UsedGlobalVarFunction, &GfunPrinter);
	freopen("F://Desktop/Function.txt", "w", stdout);
	Tool.run(newFrontendActionFactory(&FunctionFinder).get());
	FunctionFinder.~MatchFinder();

	return 0;
}
