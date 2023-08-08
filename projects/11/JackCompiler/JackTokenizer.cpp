#include "JackTokenizer.hpp"
#include <stdexcept>
#include <cctype>

JackTokenizer::JackTokenizer(string filename): source{filename}, _curToken{} {
    if (!source.is_open()) {
        throw std::runtime_error(filename + " open failed!");
    }
}

JackTokenizer::~JackTokenizer() {
    source.close();
}

void JackTokenizer::setFile(string filename) {
    source.close();
    source.open(filename);
    if (!source.is_open()) {
        throw std::runtime_error(filename + " open failed!");
    }
    _curToken = {};
}

bool JackTokenizer::hasMoreTokens() {
    char ch;
    if (source.get(ch)) {
        source.putback(ch);
        return true;
    }
    else {
        return false;
    }
}

void JackTokenizer::advance() {
    while (!getToken() && hasMoreTokens()) {
        continue;
    }
}

JackTokenizer::TokenType JackTokenizer::tokenType() {
    return _tokenType;
}

string JackTokenizer::curToken() {
    return _curToken;
}

bool JackTokenizer::getToken() {
    char ch = source.get();
    _curToken = "";
    while (std::isspace(ch) && hasMoreTokens()) {
        ch = source.get();
    }

    if (!hasMoreTokens()) {
        return false;
    }

    if (std::isdigit(ch)) {
        while (std::isdigit(ch) && hasMoreTokens()) {
            _curToken.push_back(ch);
            ch = source.get();
        }
        source.putback(ch);
        _tokenType = JackTokenizer::TokenType::INTEGER;
    }
    else if (ch == '"') {
        ch = source.get();
        while (ch != '"' && hasMoreTokens()) {
            _curToken.push_back(ch);
            ch = source.get();
        }
        _tokenType = JackTokenizer::TokenType::STRING;
    }
    else if (symbol.count(ch)) {
        if (ch == '/') {
            char next = source.get();
            if (next == '/' || next == '*'){
                ignoreComment(next);
                return false;
            }
            else {
                source.putback(next);
                _curToken.push_back(ch);
                _tokenType = JackTokenizer::TokenType::SYMBOL;
            }
        }
        else {
            _curToken.push_back(ch);
            _tokenType = JackTokenizer::TokenType::SYMBOL;
        }
    }
    else {
        while (!(symbol.count(ch) || std::isspace(ch)) && hasMoreTokens()) {
            _curToken.push_back(ch);
            ch = source.get();
        }
        source.putback(ch);
        if (keywordSet.count(_curToken)) {
            _tokenType = JackTokenizer::TokenType::KEYWORD;
        }
        else {
            _tokenType = JackTokenizer::TokenType::IDENTIFIER;
        }
    }
    return true;
}

void JackTokenizer::ignoreComment(char ch) {
    if (ch == '/') {    // "//"Comment
        while (source.get() != '\n') {
            continue;
        }
    }
    else {              // "/**/"Comment
        while ((ch = source.get()) && hasMoreTokens()) {
            if (ch == '*') {
                char next = source.get();
                if (next == '/') {
                    break;
                }
            }
        }
    }
}