#include "Parser.hpp"

#include <iostream>
#include <sstream>

ParserException::ParserException(const std::string& expected,
                                 const std::string& actual,
                                 size_t lineNumber,
                                 size_t columnNumber)
    : m_expected{expected}
    , m_actual{actual}
    , m_lineNumber{lineNumber}
    , m_columnNumber{columnNumber}
{}

Parser::Parser(Tokenizer tokenizer)
    : m_tokenizer{tokenizer}
{}

void Parser::parse()
{
    program();
}

void Parser::program()
{
    Token token = m_tokenizer.nextToken();

    // Programs are of the form BEGIN <statement list> END

    // Check for BEGIN
    if (token.data != "BEGIN") {
        throw ParserException("BEGIN", Token::TYPE_NAMES[token.type],
                              token.lineNumber, token.columnNumber);
    }

    // Parse the following statement list.
    statementList();

    // Check for END
    token = m_tokenizer.nextToken();
    if (token.data != "END") {
        throw ParserException("END", Token::TYPE_NAMES[token.type],
                              token.lineNumber, token.columnNumber);
    }
}

void Parser::statementList()
{
    // Parse a statement
    statement();

    // Parse the tail, or more statements.
    statementListTail();
}

void Parser::statementListTail()
{
    Token token = m_tokenizer.peekToken();

    // If the next token is an identifier, READ, or READ, read another statement list.
    if (token.type == Token::Type::IDENTIFIER || token.data == "READ" ||
        token.data == "WRITE") {
        statementList();
    }
}

void Parser::statement()
{
    Token token = m_tokenizer.nextToken();

    // Statements have 3 forms:
    // READ(<idList>);
    // WRITE(<exprList>);
    // <ident> := <expr>;

    // Check for READ or WRITE.
    if (token.type == Token::Type::KEYWORD) {
        // If we have a READ statement
        if (token.data == "READ") {
            // Check for left parenthesis
            token = m_tokenizer.nextToken();
            if (token.type != Token::Type::LPAREN) {
                throw ParserException("left parenthesis",
                                      Token::TYPE_NAMES[token.type],
                                      token.lineNumber, token.columnNumber);
            }
            // Parse the required id list
            idList();
            // Check for right parenthesis
            token = m_tokenizer.nextToken();
            if (token.type != Token::Type::RPAREN) {
                throw ParserException("right parenthesis",
                                      Token::TYPE_NAMES[token.type],
                                      token.lineNumber, token.columnNumber);
            }
        } else if (token.data == "WRITE") {
            // Check for left parenthesis
            token = m_tokenizer.nextToken();
            if (token.type != Token::Type::LPAREN) {
                throw ParserException("left parenthesis",
                                      Token::TYPE_NAMES[token.type],
                                      token.lineNumber, token.columnNumber);
            }
            // Parse the required expr list
            exprList();
            // Check for right parenthesis
            token = m_tokenizer.nextToken();
            if (token.type != Token::Type::RPAREN) {
                throw ParserException("right parenthesis",
                                      Token::TYPE_NAMES[token.type],
                                      token.lineNumber, token.columnNumber);
            }
        } else {
            throw ParserException("READ/WRITE", Token::TYPE_NAMES[token.type],
                                  token.lineNumber, token.columnNumber);
        }
    } else if (token.type == Token::Type::IDENTIFIER) {
        // We found an identifier, so now we need assignment and expression.
        token = m_tokenizer.nextToken();

        // Check for assignment
        if (token.type != Token::Type::ASSIGNMENT) {
            throw ParserException("assignment", Token::TYPE_NAMES[token.type],
                                  token.lineNumber, token.columnNumber);
        }

        // Parse the expression.
        expr();
    }

    token = m_tokenizer.nextToken();

    // Check if this statement ends with a semicolon. If it doesn't, throw an exception.
    if (token.data != ";") {
        // If it doesn't, throw an exception.
        throw ParserException("semicolon", Token::TYPE_NAMES[token.type],
                              token.lineNumber, token.columnNumber);
    }
}

void Parser::idList()
{
    // Check if identifier
    ident();

    // Look for additional
    idListTail();
}

void Parser::idListTail()
{
    Token token = m_tokenizer.peekToken();

    if (token.type == Token::Type::SYMBOL && token.data == ",") {
        // Found comma, so skip the token and grab next identifier list.
        m_tokenizer.nextToken();

        // Additional id list is required after comma.
        idList();
    }
}

void Parser::exprList()
{
    // Parse expression
    expr();

    // Parse additional expressions, if applicable
    exprListTail();
}

void Parser::exprListTail()
{
    Token token = m_tokenizer.peekToken();

    // If we find a comma, there's more expression lists.
    if (token.type == Token::Type::SYMBOL && token.data == ",") {
        // Found comma, so skip the token and grab next expression list.
        m_tokenizer.nextToken();

        // Parse the next expression list.
        exprList();
    }
}

void Parser::expr()
{
    // Parse a factore
    factor();

    // Parse additional expressions, if applicable.
    exprTail();
}

void Parser::exprTail()
{
    Token token = m_tokenizer.peekToken();

    // If we find another operation, there's more exprs.
    if (token.type == Token::Type::OP) {
        // Parse the OP.
        op();

        // Parse the expression
        expr();
    }
}

void Parser::factor()
{
    Token token = m_tokenizer.nextToken();

    // Factors start with a left parenthesis, identifier, or integer.
    // If left parenthesis:
    if (token.type == Token::Type::LPAREN) {
        // Parse the following expression
        expr();

        token = m_tokenizer.nextToken();

        // Require right parenthesis.
        if (token.type != Token::Type::RPAREN) {
            throw ParserException("RPAREN", Token::TYPE_NAMES[token.type],
                                  token.lineNumber, token.columnNumber);
        }
    } else if (token.type == Token::Type::IDENTIFIER) {
        // Nothing to do, got what we expected
    } else if (token.type == Token::Type::INTEGER) {
        // Nothing to do, got what we expected.
    } else {
        throw ParserException("INTEGER or IDENTIFIER", Token::TYPE_NAMES[token.type],
                              token.lineNumber, token.columnNumber);
    }
}

void Parser::op()
{
    Token token = m_tokenizer.nextToken();

    // If the next token isn't an operation throw an exception.
    if (token.type != Token::Type::OP) {
        throw ParserException("OPERATION", Token::TYPE_NAMES[token.type],
                              token.lineNumber, token.columnNumber);
    }
}

void Parser::ident()
{
    Token token = m_tokenizer.nextToken();

    // If the next token isn't an identifier throw an exception.
    if (token.type != Token::Type::IDENTIFIER) {
        throw ParserException("IDENTIFIER", Token::TYPE_NAMES[token.type],
                              token.lineNumber, token.columnNumber);
    }
}
