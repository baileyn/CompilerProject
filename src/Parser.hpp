#ifndef PARSER_HPP
#define PARSER_HPP

#include "Tokenizer.hpp"

#include <exception>
#include <string>

class ParserException : public std::exception
{
public:
    ParserException(const std::string& expected,
                    const std::string& actual,
                    size_t lineNumber,
                    size_t columnNumber);

    std::string expected() const
    {
        return m_expected;
    }

    std::string actual() const
    {
        return m_actual;
    }

    size_t lineNumber() const
    {
        return m_lineNumber;
    }

    size_t columnNumber() const
    {
        return m_columnNumber;
    }

private:
    std::string m_expected;
    std::string m_actual;
    size_t m_lineNumber;
    size_t m_columnNumber;
};

class Parser
{
public:
    Parser(Tokenizer tokenizer);

    void parse();

private:
    void program();
    void statementList();
    void statementListTail();
    void statement();
    void idList();
    void idListTail();
    void exprList();
    void exprListTail();
    void expr();
    void exprTail();
    void factor();
    void op();
    void ident();

private:
    Tokenizer m_tokenizer;
};

#endif // PARSER_HPP
