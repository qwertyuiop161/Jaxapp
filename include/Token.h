#pragma once
#include <string>
enum class TokenType {
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Comma,
    Dot,
    Semicolon,
    Colon,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Equal,
    EqualEqual,
    Bang,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    AndAnd,
    Or,
    OrOr,
    PlusPlus,
    MinusMinus,
    Identifier,
    Integer,
    Float,
    String,
    Character,
    Fn,
    Class,
    Return,
    If,
    Else,
    While,
    For,
    Break,
    Continue,
    True,
    False,
    Null,
    This,
    New,
    Int,
    FloatType,
    Bool,
    Char,
    StringType,
    Void,
    EndOfFile,
    Unknown
};
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    Token(
        TokenType type,
        std::string lexeme,
        int line,
        int column
    )
        :
        type(type),
        lexeme(lexeme),
        line(line),
        column(column)
    {}
};
std::string tokenTypeToString(TokenType type);