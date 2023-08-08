#include <string>
#include "CompilationEngine.hpp"

using std::string;

CompilationEngine::CompilationEngine(string input, string output):
_jackTokenizer{input}, _VMWriter{output}, _subroutineVar{}, _classVar{}, _curClass{}
{
    _jackTokenizer.advance();
}

CompilationEngine::~CompilationEngine() { }

void CompilationEngine::setFile(string input, string output) {
    _jackTokenizer.setFile(input);
    _jackTokenizer.advance();
    _VMWriter.setFile(output);
    _classVar.reset();
    _subroutineVar.reset();
}

/* class: 'class' className '{' classVarDec* subroutineDec* '}' */
void CompilationEngine::compileClass() {
    // compile token: 'class' className '{'
    _jackTokenizer.advance();               // ignore 'class'
    _curClass = _jackTokenizer.curToken();  // curClass = className
    _jackTokenizer.advance();
    _jackTokenizer.advance();               // ignore '{'

    // compile token: classVarDec*
    while (_jackTokenizer.curToken() == "static" || _jackTokenizer.curToken() == "field") {
        compileClassVarDec();
    }

    // compile token: subroutineDec*
    while (_jackTokenizer.curToken() == "constructor" || _jackTokenizer.curToken() == "function" || _jackTokenizer.curToken() == "method") {
        compileSubroutine();
    }

    // compile token: '}'
    _jackTokenizer.advance();               // ignore '}'
}

/* classVarDec: ('static' | 'field') type varName (',' varName)* ';' */
void CompilationEngine::compileClassVarDec() {
    // compile token: ('static' | 'field') type VarName
    SymbolTable::SymbolKind kind;
    string type, name;
    kind = _jackTokenizer.curToken() == "static" ? SymbolTable::SymbolKind::STATIC : SymbolTable::SymbolKind::FIELD;
    _jackTokenizer.advance();
    type = _jackTokenizer.curToken();
    _jackTokenizer.advance();
    name = _jackTokenizer.curToken();
    _jackTokenizer.advance();
    _classVar.define(name, type, kind);

    // compile token: (',' varName)*
    while (_jackTokenizer.curToken() == ",") {
        _jackTokenizer.advance();               // ignore ','
        name = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        _classVar.define(name, type, kind);
    }

    // compile token: ';'
    _jackTokenizer.advance();                   // ignore ';'
}

/* subroutineDec: ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody */
/* subroutineBody: '{' varDec* statements '}' */
void CompilationEngine::compileSubroutine() {
    // reset subroutineSymbolTable
    _subroutineVar.reset();

    // compile token: ('constructor' | 'function' | 'method') ('void' | type) subroutineName '('
    string functionName, functionType;
    functionType = _jackTokenizer.curToken();
    _jackTokenizer.advance();
    _jackTokenizer.advance();                   // ignore ('void' | type)
    functionName = _jackTokenizer.curToken();   // functionName = subroutineName
    _jackTokenizer.advance();
    _jackTokenizer.advance();                   // ignore '('

    if (functionType == "method") {
        _subroutineVar.define("this", _curClass, SymbolTable::SymbolKind::ARG);
    }
    // compile token: parameterList
    compileParameterList();

    // compile token: ')'
    _jackTokenizer.advance();                   // ignore ')'

    // compile token: '{'
    _jackTokenizer.advance();                   // ignore '{'
    //compile token: varDec*
    while (_jackTokenizer.curToken() == "var") {
        compileVarDec();
    }

    // VMWriter: function className.functionName localVarCount
    _VMWriter.writeFunction(_curClass + '.' + functionName, _subroutineVar.varCount(SymbolTable::SymbolKind::VAR));
    if (functionType == "constructor") {
        // VMWriter: push constant classVarCount
        _VMWriter.writePush("constant", _classVar.varCount(SymbolTable::SymbolKind::FIELD));
        // VMWriter: call Memory.alloc 1
        _VMWriter.writeCall("Memory.alloc", 1);
        // VMWriter: pop pointer 0
        _VMWriter.writePop("pointer", 0);
    }
    else if (functionType == "method") {
        // VMWriter: push argument 0
        _VMWriter.writePush("argument", 0);
        // VMWriter: pop pointer 0
        _VMWriter.writePop("pointer", 0);
    }

    // compiler statements
    compileStatements();

    // compiler '}'
    _jackTokenizer.advance();                   // ignore '}'
}

/* parameterList: ((type varName) (',' type varName)*)? */
void CompilationEngine::compileParameterList() {
    if (_jackTokenizer.curToken() != ")") {
        string type, name;
        // compile token: type VarName
        type = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        name = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        _subroutineVar.define(name, type, SymbolTable::SymbolKind::ARG);

        // compile token: (',' type varName)*
        while (_jackTokenizer.curToken() == ",") {
            _jackTokenizer.advance();                   // ignore ','
            type = _jackTokenizer.curToken();
            _jackTokenizer.advance();
            name = _jackTokenizer.curToken();
            _jackTokenizer.advance();
            _subroutineVar.define(name, type, SymbolTable::SymbolKind::ARG);
        }
    }
}

/* varDec: 'var' type varName (',' varName)* ';' */
void CompilationEngine::compileVarDec() {
    string type, name;
    // compile token: 'var' type varName
    _jackTokenizer.advance();               // ignore 'var'
    type = _jackTokenizer.curToken();
    _jackTokenizer.advance();
    name = _jackTokenizer.curToken();
    _jackTokenizer.advance();
    _subroutineVar.define(name, type, SymbolTable::SymbolKind::VAR);

    // compile token: (',' varName)*
    while (_jackTokenizer.curToken() == ",") {
        _jackTokenizer.advance();           //ignore ','
        name = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        _subroutineVar.define(name, type, SymbolTable::SymbolKind::VAR);
    }

    // compile token: ';'
    _jackTokenizer.advance();               // ignore ';'
}

/* statements: statement* */
void CompilationEngine::compileStatements() {
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
}

/* letStatement: 'let' varName ('[' expression ']')? '=' expression ';' */
void CompilationEngine::compileLet() {
    string varName;
    bool arrFlag = false;
    // compile token: 'let' varName
    _jackTokenizer.advance();               // ignore 'let'
    varName = _jackTokenizer.curToken();
    _jackTokenizer.advance();

    // compile token: ('[' expression ']')?
    // ToDo
    if (_jackTokenizer.curToken() == "[") {
        arrFlag = true;
        // VMWriter: push varName
        if (_subroutineVar.exist(varName)) {
            _VMWriter.writePush(_subroutineVar.kindOf(varName)==SymbolTable::SymbolKind::ARG ? "argument" : "local",
            _subroutineVar.indexOf(varName));
        }
        else if (_classVar.exist(varName)) {
            _VMWriter.writePush(_classVar.kindOf(varName)==SymbolTable::SymbolKind::STATIC ? "static" : "this",
            _classVar.indexOf(varName));
        }
        _jackTokenizer.advance();           // ignore '['
        compileExpression();
        _jackTokenizer.advance();           // ignore ']'
        // VMWriter: add
        _VMWriter.writeArithmetic("add");
    }

    // compile token: '=' expression ';'
    _jackTokenizer.advance();               // ignore '='
    compileExpression();
    _jackTokenizer.advance();               // ignore ';'

    if (arrFlag) {
        // VMWriter: pop temp 0
        _VMWriter.writePop("temp", 0);
        // VMWriter: pop pointer 1
        _VMWriter.writePop("pointer", 1);
        // VMWriter: push temp 0
        _VMWriter.writePush("temp", 0);
        // VMWriter: pop that 0
        _VMWriter.writePop("that", 0);
    }
    else {
        // VMWriter: pop varName
        if (_subroutineVar.exist(varName)) {
            _VMWriter.writePop(_subroutineVar.kindOf(varName)==SymbolTable::SymbolKind::ARG ? "argument" : "local",
            _subroutineVar.indexOf(varName));
        }
        else if (_classVar.exist(varName)) {
            _VMWriter.writePop(_classVar.kindOf(varName)==SymbolTable::SymbolKind::STATIC ? "static" : "this",
            _classVar.indexOf(varName));
        }
    }
}

/* ifStatement: 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')? */
void CompilationEngine::compileIf() {
    int iflabelIndex = labelCount;
    labelCount = labelCount + 2;
    // compile token: 'if' '('
    _jackTokenizer.advance();               // ignore 'if'
    _jackTokenizer.advance();               // ignore '('

    // compile token: expression
    compileExpression();
    // VMWriter: not
    _VMWriter.writeArithmetic("not");
    // VMWriter: if-goto L1
    _VMWriter.writeIf("_if_label_" + std::to_string(iflabelIndex));

    // compile token: ')' '{'
    _jackTokenizer.advance();               // ignore ')'
    _jackTokenizer.advance();               // ignore '{'

    // compile token: statements
    compileStatements();

    // compile token: '}'
    _jackTokenizer.advance();               // ignore '}'

    //compile token: ('else' '{' statements '}')?
    if (_jackTokenizer.curToken() == "else") {
        // VMWriter: goto L2
        _VMWriter.writeGoto("_if_label_" + std::to_string(iflabelIndex+1));
        // VMWriter: label L1
        _VMWriter.writeLabel("_if_label_" + std::to_string(iflabelIndex));
        _jackTokenizer.advance();           // ignore 'else'
        _jackTokenizer.advance();           // ignore '{'
        compileStatements();
        _jackTokenizer.advance();           // ignore '}'
        // VMWriter: label L2
        _VMWriter.writeLabel("_if_label_" + std::to_string(iflabelIndex+1));
    }
    else {
        // VMWriter: label L1
        _VMWriter.writeLabel("_if_label_" + std::to_string(iflabelIndex));
        labelCount -= 1;
    }
}

/* whileStatement: 'while' '(' expression ')' '{' statements'}' */
void CompilationEngine::compileWhile() {
    int whilelabelIndex = labelCount;
    labelCount = labelCount + 2;
    // compile token: 'while' '('
    _jackTokenizer.advance();               // ignore 'while'
    _jackTokenizer.advance();               // ignore '('

    // VMWriter: label L1
    _VMWriter.writeLabel("_while_label_" + std::to_string(whilelabelIndex));

    // compile token: expression
    compileExpression();
    // VMWriter: not
    _VMWriter.writeArithmetic("not");
    // VMWriter: if-goto L2
    _VMWriter.writeIf("_while_label_" + std::to_string(whilelabelIndex+1));

    // compile token: ')' '{'
    _jackTokenizer.advance();               // ignore ')'
    _jackTokenizer.advance();               // ignore '{'

    // compile token: statements
    compileStatements();

    // VMWriter: goto L1
    _VMWriter.writeGoto("_while_label_" + std::to_string(whilelabelIndex));
    // VMWriter: label L2
    _VMWriter.writeLabel("_while_label_" + std::to_string(whilelabelIndex+1));

    // compile token: '}'
    _jackTokenizer.advance();               // ignore '}'
}

/* doStatement: 'do' subroutineCall ';' */
/* subroutineCall: subroutineName '(' expressionList ')'|(className|varName) '.' subroutineName '(' expressionList ')' */
void CompilationEngine::compileDo() {
    // compile token: 'do'
    _jackTokenizer.advance();                   // ignore 'do'

    // compile token: subroutineCall
    string tk = _jackTokenizer.curToken();
    _jackTokenizer.advance();
    if (_jackTokenizer.curToken() == ".") {
        // compile token: '.'
        _jackTokenizer.advance();           // ignore '.'
        if (_subroutineVar.exist(tk)) {
            _VMWriter.writePush(_subroutineVar.kindOf(tk) == SymbolTable::SymbolKind::ARG ? "argument" : "local"
            , _subroutineVar.indexOf(tk));
        }
        else if (_classVar.exist(tk)) {
            _VMWriter.writePush(_classVar.kindOf(tk) == SymbolTable::SymbolKind::STATIC ? "static" : "this"
            , _classVar.indexOf(tk));
        }
        // compile token: subroutineName
        string functionName = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        // compile token: '('
        _jackTokenizer.advance();           // ignore '('
        //  compile token: expressionList
        int nArgs = compileExpressionList();
        // compile token: ')'
        _jackTokenizer.advance();           // ignore ')'
        // VMWriter: call className.functionName nArgs
        if (_subroutineVar.exist(tk)) {
            _VMWriter.writeCall(_subroutineVar.typeOf(tk)+'.'+functionName, nArgs+1);
        }
        else if (_classVar.exist(tk)) {
            _VMWriter.writeCall(_classVar.typeOf(tk)+'.'+functionName, nArgs+1);
        }
        else {
            _VMWriter.writeCall(tk+'.'+functionName, nArgs);
        }
    }
    else if (_jackTokenizer.curToken() == "(") {
        // compile token: '('
        _jackTokenizer.advance();           // ignore '('
        _VMWriter.writePush("pointer", 0);
        int nArgs = compileExpressionList() + 1;
        // VMWriter: call tk nArgs
        _VMWriter.writeCall(_curClass+'.'+tk, nArgs);
        // compile token: ')'
        _jackTokenizer.advance();           // ignore ')'
    }
    // VMWriter: pop temp 0
    _VMWriter.writePop("temp", 0);

    // compile token: ';'
    _jackTokenizer.advance();           // ignore ';'
}

/* returnStatement: 'return' expression? ';' */
void CompilationEngine::compileReturn() {
    // compile token: 'return'
    _jackTokenizer.advance();                   // ignore 'return'

    // compile token: expression?
    if (_jackTokenizer.curToken() != ";") {
        compileExpression();
    }
    else {
        // VMWrtier: push constant 0
        _VMWriter.writePush("constant", 0);
    }

    // compile token: ';'
    _jackTokenizer.advance();                   // ignore ';'

    // VMWriter: return
    _VMWriter.writeReturn();
}

/* expression: term(op term)* */
void CompilationEngine::compileExpression() {
    // compile token: term
    compileTerm();

    while (opSet.count(_jackTokenizer.curToken())) {
        string op = _jackTokenizer.curToken();
        _jackTokenizer.advance();
        // compile token: op term
        compileTerm();
        if (op == "*") {
            // VMWriter: call Math.multiply 2
            _VMWriter.writeCall("Math.multiply", 2);
        }
        else if (op == "/") {
            // VMWriter: call Math.divide 2
            _VMWriter.writeCall("Math.divide", 2);
        }
        else {
            // VMWriter: op
            _VMWriter.writeArithmetic(arithmeticMap.at(op));
        }
    }
}

/* term: integerConstant|stringConstant|keywordConstant|varName|varName'['expreesion']'|
 * '('expression')'|(unaryOp term)|subroutineCall
 */
/* subroutineCall: subroutineName '(' expressionList ')'|(className|varName) '.' subroutineName '(' expressionList ')' */
void CompilationEngine::compileTerm() {
    // compile token: term
    string tk = _jackTokenizer.curToken();
    JackTokenizer::TokenType tkType = _jackTokenizer.tokenType();
    _jackTokenizer.advance();
    if (tkType == JackTokenizer::TokenType::INTEGER) {
        // compile token: integerConstant
        // VMWriter: push constant term
        _VMWriter.writePush("constant", std::stoi(tk));
    }
    else if (tkType == JackTokenizer::TokenType::KEYWORD) {
        // compile token: keywordConstant
        if (tk == "true") {
            // VMWriter: push constant 1
            _VMWriter.writePush("constant", 1);
            // VMWriter: neg
            _VMWriter.writeArithmetic("neg");
        }
        else if (tk == "false" || tk == "null") {
            // VMWriter: push constant 0
            _VMWriter.writePush("constant", 0);
        }
        else if (tk == "this") {
            // VMWriter: push pointer 0
            _VMWriter.writePush("pointer", 0);
        }
    }
    else if (tkType == JackTokenizer::TokenType::STRING) {
        // compile token: stringConstant
        // _VMWriter: push constant tk.length()
        _VMWriter.writePush("constant", tk.length());
        // _VMWriter: call String.new 1
        _VMWriter.writeCall("String.new", 1);
        for (auto ch : tk) {
            // _VMWriter: push constant ch
            _VMWriter.writePush("constant", ch);
            // _VMWriter: call String.appendChar 2
            _VMWriter.writeCall("String.appendChar", 2);
        }
    }
    else if (tkType == JackTokenizer::TokenType::SYMBOL) {
        if (tk == "-") {
            compileTerm();
            // VMWriter: neg
            _VMWriter.writeArithmetic("neg");
        }
        else if (tk == "~") {
            compileTerm();
            // VMWriter: not
            _VMWriter.writeArithmetic("not");
        }
        else if (tk == "(") {
            // compile token: expression
            compileExpression();
            // compile token: ')'
            _jackTokenizer.advance();           // ignore ')'
        }
    }
    else {
        if (_jackTokenizer.curToken() == "[") {
            // compile token: '['
            _jackTokenizer.advance();           // ignore '['
            // compile token: expression
            compileExpression();
            // compile token: ']'
            _jackTokenizer.advance();           // ignore ']'
            // VMWriter: push tk
            if (_subroutineVar.exist(tk)) {
                _VMWriter.writePush(_subroutineVar.kindOf(tk) == SymbolTable::SymbolKind::ARG ? "argument" : "local"
                , _subroutineVar.indexOf(tk));
            }
            else if (_classVar.exist(tk)) {
                _VMWriter.writePush(_classVar.kindOf(tk) == SymbolTable::SymbolKind::STATIC ? "static" : "this"
                , _classVar.indexOf(tk));
            }
            // VMWriter: add
            _VMWriter.writeArithmetic("add");
            // VMWriter: pop pointer 1
            _VMWriter.writePop("pointer", 1);
            // VMWriter: push that 0
            _VMWriter.writePush("that", 0);
        }
        else if (_jackTokenizer.curToken() == ".") {
            // compile token: '.'
            _jackTokenizer.advance();           // ignore '.'
            if (_subroutineVar.exist(tk)) {
                _VMWriter.writePush(_subroutineVar.kindOf(tk) == SymbolTable::SymbolKind::ARG ? "argument" : "local"
                , _subroutineVar.indexOf(tk));
            }
            else if (_classVar.exist(tk)) {
                _VMWriter.writePush(_classVar.kindOf(tk) == SymbolTable::SymbolKind::STATIC ? "static" : "this"
                , _classVar.indexOf(tk));
            }
            // compile token: subroutineName
            string functionName = _jackTokenizer.curToken();
            _jackTokenizer.advance();
            // compile token: '('
            _jackTokenizer.advance();           // ignore '('
            //  compile token: expressionList
            int nArgs = compileExpressionList();
            // compile token: ')'
            _jackTokenizer.advance();           // ignore ')'
            // VMWriter: call className.functionName nArgs
            if (_subroutineVar.exist(tk)) {
                _VMWriter.writeCall(_subroutineVar.typeOf(tk)+'.'+functionName, nArgs+1);
            }
            else if (_classVar.exist(tk)) {
                _VMWriter.writeCall(_classVar.typeOf(tk)+'.'+functionName, nArgs+1);
            }
            else {
                _VMWriter.writeCall(tk+'.'+functionName, nArgs);
            }
        }
        else if (_jackTokenizer.curToken() == "(") {
            // compile token: '('
            _jackTokenizer.advance();           // ignore '('
            _VMWriter.writePush("pointer", 0);
            int nArgs = compileExpressionList() + 1;
            // VMWriter: call tk nArgs
            _VMWriter.writeCall(_curClass+'.'+ tk, nArgs);
            // compile token: ')'
            _jackTokenizer.advance();           // ignore ')'
        }
        else {
            // VMWriter: push tk
            if (_subroutineVar.exist(tk)) {
                _VMWriter.writePush(_subroutineVar.kindOf(tk) == SymbolTable::SymbolKind::ARG ? "argument" : "local"
                , _subroutineVar.indexOf(tk));
            }
            else if (_classVar.exist(tk)) {
                _VMWriter.writePush(_classVar.kindOf(tk) == SymbolTable::SymbolKind::STATIC ? "static" : "this"
                , _classVar.indexOf(tk));
            }
        }
    }
}

/* expressionList: (expression (',' expression)*)? */
int CompilationEngine::compileExpressionList() {
    int count{};
    string token = _jackTokenizer.curToken();
    JackTokenizer::TokenType tkType = _jackTokenizer.tokenType();
    if (tkType != JackTokenizer::TokenType::SYMBOL || token == "-" || token == "~" || token == "(") {
        count++;
        // compile token: expression
        compileExpression();
        // compile token: (',' expression)*
        while (_jackTokenizer.curToken() == ",") {
            _jackTokenizer.advance();               // ignore ','
            compileExpression();
            count++;
        }
    }
    return count;
}
