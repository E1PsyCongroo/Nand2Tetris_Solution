#include <string>
#include "CompilationEngine.hpp"

using std::string;

CompilationEngine::CompilationEngine(string input, string output): _jackTokenizer{input}, ofs{output} {
    if (!ofs.is_open()) {
        throw std::runtime_error(output + " open failed");
    };
    _jackTokenizer.advance();
}

CompilationEngine::~CompilationEngine() {
    ofs.close();
}

void CompilationEngine::setFile(string input, string output) {
    _jackTokenizer.setFile(input);
    ofs.close();
    ofs.open(output);
    if (!ofs.is_open()) {
        throw std::runtime_error(output + " open failed");
    };
    _jackTokenizer.advance();
}

void CompilationEngine::compileTokens() {
    ofs << XMLstart("tokens") << '\n';
    while (_jackTokenizer.hasMoreTokens()) {
        auto type = typeMap[_jackTokenizer.tokenType()];
        ofs << XMLstart(type)
            << ' '
            << getToken()
            << ' '
            << XMLend(type)
            << '\n';
    }
    ofs <<XMLend("tokens") << '\n';
}

void CompilationEngine::compileToken() {
    auto token = _jackTokenizer.curToken();
    if (symbolMap.count(token)) {
        token = symbolMap[token];
    }
    ofs << XMLstart(typeMap[_jackTokenizer.tokenType()])
        << ' ' << token << ' '
        << XMLend(typeMap[_jackTokenizer.tokenType()])
        << '\n';
    _jackTokenizer.advance();
}

/* class: 'class' className '{' classVarDec* subroutineDec* '}' */
void CompilationEngine::compileClass() {
    ofs << XMLstart("class") << '\n';

    // compile token: 'class' className '{'
    for (int i = 0; i < 3; i++) {
        compileToken();
    }

    // compile token: classVarDec*
    while (_jackTokenizer.curToken() == "static" || _jackTokenizer.curToken() == "field") {
        compileClassVarDec();
    }

    // compile token: subroutineDec*
    while (_jackTokenizer.curToken() == "constructor" || _jackTokenizer.curToken() == "function" || _jackTokenizer.curToken() == "method") {
        compileSubroutine();
    }

    // compile token: '}'
    compileToken();

    ofs <<XMLend("class") << '\n';
}

/* classVarDec: ('static' | 'field') type varName (',' varName)* ';' */
void CompilationEngine::compileClassVarDec() {
    ofs << XMLstart("classVarDec") << '\n';

    // compile token: ('static' | 'field') type varName
    for (int i = 0; i < 3; i++) {
        compileToken();
    }

    // compile token: (',' varName)*
    while (_jackTokenizer.curToken() == ",") {
        for (int i = 0; i < 2; i++) {
            compileToken();
        }
    }

    // compile token: ';'
    compileToken();

    ofs <<XMLend("classVarDec") << '\n';
}

/* subroutineDec: ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody */
void CompilationEngine::compileSubroutine() {
    ofs << XMLstart("subroutineDec") << '\n';

    // compile token: ('constructor' | 'function' | 'method') ('void' | type) subroutineName '('
    for (int i = 0; i < 4; i++) {
        compileToken();
    }

    // compile token: parameterList
    compileParameterList();

    // compile token: ')'
    compileToken();

    // compile token: subroutineBody
    compileSubroutineBody();

    ofs <<XMLend("subroutineDec") << '\n';
}

/* parameterList: ((type varName) (',' type varName)*)? */
void CompilationEngine::compileParameterList() {
    ofs << XMLstart("parameterList") << '\n';

    if (_jackTokenizer.curToken() != ")") {
        // compile token: type VarName
        for (int i = 0; i < 2; i++) {
            compileToken();
        }
        // compile token: (',' type varName)*
        while (_jackTokenizer.curToken() == ",") {
            for (int i = 0; i < 3; i++) {
                compileToken();
            }
        }
    }

    ofs << XMLend("parameterList") << '\n';
}

/* subroutineBody: '{' varDec* statements '}' */
void CompilationEngine::compileSubroutineBody() {
    ofs << XMLstart("subroutineBody") << '\n';

    // compile token: '{'
    compileToken();

    //compile token: varDec*
    while (_jackTokenizer.curToken() == "var") {
        compileVarDec();
    }

    //compile token: statements*
    compileStatements();

    // compile token: '}'
    compileToken();

    ofs << XMLend("subroutineBody") << '\n';
}

/* varDec: 'var' type varName (',' varName)* ';' */
void CompilationEngine::compileVarDec() {
    ofs << XMLstart("varDec") << '\n';

    // compile token: 'var' type varName
    for (int i = 0; i < 3; i++) {
        compileToken();
    }

    // compile token: (',' varName)*
    while (_jackTokenizer.curToken() == ",") {
        for (int i = 0; i < 2; i++) {
            compileToken();
        }
    }

    // compile token: ';'
    compileToken();

    ofs << XMLend("varDec") << '\n';
}

/* statements: statement* */
void CompilationEngine::compileStatements() {
    ofs << XMLstart("statements") << '\n';

    /* statement: letStatement | ifStatement | whileStatement | doStatement | returnStatement*/
    while (true) {
        auto token = _jackTokenizer.curToken();
        if (token == "let") {
            compileLet();
        }
        else if (token == "if") {
            compileIf();
        }
        else if (token == "while") {
            compileWhile();
        }
        else if (token == "do") {
            compileDo();
        }
        else if (token == "return") {
            compileReturn();
        }
        else {
            break;
        }
    }

    ofs << XMLend("statements") << '\n';
}

/* letStatement: 'let' varName ('[' expression ']')? '=' expression ';' */
void CompilationEngine::compileLet() {
    ofs << XMLstart("letStatement") << '\n';

    // compile token: 'let' varName
    for (int i = 0; i < 2; i++) {
        compileToken();
    }

    // compile token: ('[' expression ']')?
    if (_jackTokenizer.curToken() == "[") {
        compileToken();
        compileExpression();
        compileToken();
    }

    // compile token: '=' expression ';'
    compileToken();
    compileExpression();
    compileToken();

    ofs << XMLend("letStatement") << '\n';
}

/* ifStatement: 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')? */
void CompilationEngine::compileIf() {
    ofs << XMLstart("ifStatement") << '\n';

    // compile token: 'if' '('
    for (int i = 0; i < 2; i++) {
        compileToken();
    }

    // compile token: expression
    compileExpression();

    // compile token: ')' '{'
    for (int i = 0; i < 2; i++) {
        compileToken();
    }

    // compile token: statements
    compileStatements();

    // compile token: '}'
    compileToken();

    //compile token: ('else' '{' statements '}')?
    if (_jackTokenizer.curToken() == "else") {
        for (int i = 0; i < 2; i++) {
            compileToken();
        }
        compileStatements();
        compileToken();
    }

    ofs << XMLend("ifStatement") << '\n';
}

/* whileStatement: 'while' '(' expression ')' '{' statements'}' */
void CompilationEngine::compileWhile() {
    ofs << XMLstart("whileStatement") << '\n';

    // compile token: 'while' '('
    for (int i = 0; i < 2; i++) {
        compileToken();
    }

    // compile token: expression
    compileExpression();

    // compile token: ')' '{'
    for (int i = 0; i < 2; i++) {
        compileToken();
    }

    // compile token: statements
    compileStatements();

    // compile token: '}'
    compileToken();

    ofs << XMLend("whileStatement") << '\n';
}

/* doStatement: 'do' subroutineCall ';' */
/* subroutineCall: subroutineName '(' expressionList ')'|(className|varName) '.' subroutineName '(' expressionList ')' */
void CompilationEngine::compileDo() {
    ofs << XMLstart("doStatement") << '\n';

    // compile token: 'do'
    compileToken();

    // compile token: subroutineCall
    compileToken();
    if (_jackTokenizer.curToken() == "(") {
        // compile token: '(' expressionList ')'
        compileToken();
        compileExpressionList();
        compileToken();
    }
    else {
        // compile token: '.' subroutineName '(' expressionList ')'
        compileToken();
        compileToken();
        compileToken();
        compileExpressionList();
        compileToken();
    }

    // compile token: ';'
    compileToken();

    ofs << XMLend("doStatement") << '\n';
}

/* returnStatement: 'return' expression? ';' */
void CompilationEngine::compileReturn() {
    ofs << XMLstart("returnStatement") << '\n';

    // compile token: 'return'
    compileToken();

    // compile token: expression?
    if (_jackTokenizer.curToken() != ";") {
        compileExpression();
    }

    // compile token: ';'
    compileToken();

    ofs << XMLend("returnStatement") << '\n';
}

/* expression: term(op term)* */
void CompilationEngine::compileExpression() {
    ofs << XMLstart("expression") << '\n';

    // compile token: term
    compileTerm();
    while (opSet.count(_jackTokenizer.curToken())) {
        // compile token: op term
        compileToken();
        compileTerm();
    }

    ofs << XMLend("expression") << '\n';
}

/* term: intergerConstant|stringConstant|keywordConstant|varName|varName'['expreesion']'|
 * '('expression')'|(unaryOp term)|subroutineCall
 */
/* subroutineCall: subroutineName '(' expressionList ')'|(className|varName) '.' subroutineName '(' expressionList ')' */
void CompilationEngine::compileTerm() {
    ofs << XMLstart("term") << '\n';

    // compile token: term
    if (_jackTokenizer.curToken() == "(") {
        compileToken();
        compileExpression();
        compileToken();
    }
    else {
        // compile token: unaryOp term
        if (unaryOpSet.count(_jackTokenizer.curToken())) {
            compileToken();
            compileTerm();
        }
        else {
            compileToken();
        }
        // compile token: varName '[' expression ']'
        if (_jackTokenizer.curToken() == "[") {
            compileToken();
            compileExpression();
            compileToken();
        }
        // compile token: subroutineCall
        else if (_jackTokenizer.curToken() == "("){
            // compile token: '(' expressionList ')'
            compileToken();
            compileExpressionList();
            compileToken();
        }
        else if (_jackTokenizer.curToken() == ".") {
            // compile token: '.' subroutineName '(' expressionList ')'
            compileToken();
            compileToken();
            compileToken();
            compileExpressionList();
            compileToken();
        }
    }

    ofs << XMLend("term") << '\n';
}

/* expressionList: (expression (',' expression)*)? */
void CompilationEngine::compileExpressionList() {
    ofs << XMLstart("expressionList") << '\n';

    string token = _jackTokenizer.curToken();
    if (!(opSet.count(token) || token == "[" || token == "]" || token == "{" || token == ")" || token == "}")) {
        // compile token: expression
        compileExpression();
        // compile token: (',' expression)*
        while (_jackTokenizer.curToken() == ",") {
            compileToken();
            compileExpression();
        }
    }

    ofs << XMLend("expressionList") << '\n';
}
