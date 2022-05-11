#include "Tokenizer.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

// Create a mapping for human readable names.
std::map<Token::Type, std::string> Token::TYPE_NAMES{
    std::make_pair(Token::Type::IDENTIFIER, "IDENTIFIER"),
    std::make_pair(Token::Type::INTEGER, "INTEGER"),
    std::make_pair(Token::Type::KEYWORD, "KEYWORD"),
    std::make_pair(Token::Type::WHITESPACE, "WHITESPACE"),
    std::make_pair(Token::Type::LPAREN, "LPAREN"),
    std::make_pair(Token::Type::RPAREN, "RPAREN"),
    std::make_pair(Token::Type::SYMBOL, "SYMBOL"),
    std::make_pair(Token::Type::INTEGER, "INTEGER"),
    std::make_pair(Token::Type::OP, "OPERATION"),
    std::make_pair(Token::Type::ASSIGNMENT, "ASSIGNMENT"),
    std::make_pair(Token::Type::TEOF, "EOF"),
    std::make_pair(Token::Type::UNKNOWN, "UNRECOGNIZED TOKEN"),
};

// A list of symbols
std::vector<std::string> Tokenizer::SYMBOLS{",", ";"};

// A list of keywords
std::vector<std::string> Tokenizer::KEYWORDS{"BEGIN", "END", "READ", "WRITE"};

Tokenizer::Tokenizer()
    : m_lineNumber{1}
    , m_columNumber{1}
    , m_index{0}
{}

bool Tokenizer::loadFile(const std::string& fileName)
{
    // Open the file stream to read from.
    std::ifstream fileStream{fileName};

    // If we were unable to open the file, return false.
    if (!fileStream.is_open()) {
        return false;
    }

    std::string contents{std::istreambuf_iterator<char>(fileStream),
                         std::istreambuf_iterator<char>()};

    m_source = contents;
    m_index = 0;
    m_lineNumber = 1;
    m_columNumber = 1;

    // Load all of the tokens into the queue.
    loadTokens();

    return true;
}

Token Tokenizer::nextToken()
{
    // Retrieve and remove the front token.
    Token token = m_tokens.front();
    m_tokens.pop();

    return token;
}

Token Tokenizer::peekToken()
{
    // Retrieve the front token, but don't remove it.
    return m_tokens.front();
}

Token Tokenizer::readNextToken()
{
    // If the index is passed the end of the source code, return EOF.
    if (m_index >= m_source.length()) {
        return Token(Token::Type::TEOF, "", m_lineNumber, m_columNumber);
    }

    Token token;

    // Attempt to read whitespace
    push();
    if (readWhitespace(token)) {
        return token;
    } else {
        pop();
    }

    // Attempt to read an identifier
    push();
    if (readIdentifier(token)) {
        // Check if the identifier is a reserved keyword, if so change the token type.
        if (std::find(std::begin(KEYWORDS), std::end(KEYWORDS), token.data) !=
            std::end(KEYWORDS)) {
            token.type = Token::Type::KEYWORD;
        }

        return token;
    } else {
        pop();
    }

    // Attempt to read a symbol
    push();
    if (readSymbol(token)) {
        return token;
    } else {
        pop();
    }

    // Attempt to read an integer
    push();
    if (readInteger(token)) {
        return token;
    } else {
        pop();
    }

    // Attempt to read a left or right parenthesis.
    push();
    if (readParens(token)) {
        return token;
    } else {
        pop();
    }

    // Attempt to read the assignment operator.
    push();
    if (readAssignment(token)) {
        return token;
    } else {
        pop();
    }

    // Attempt to read an operation (+/-)
    push();
    if (readOp(token)) {
        return token;
    } else {
        pop();
    }

    token = Token(Token::Type::UNKNOWN, "", m_lineNumber, m_columNumber);

    // Make index out of range to stop parsing tokens.
    m_index = std::numeric_limits<size_t>::max();

    return token;
}

bool Tokenizer::readWhitespace(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // While the next character is whitespace, add it to the token data.
    while (std::iswspace(peek())) {
        data += next();
    }

    // Construct the token
    if (!data.empty()) {
        outputToken.type = Token::Type::WHITESPACE;
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

bool Tokenizer::readIdentifier(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // Make sure the first character is a letter
    if (std::isalpha(peek())) {
        data += next();

        // Read while it's a letter, numeric, or underscore.
        while (std::isalnum(peek()) || peek() == '_') {
            data += next();
        }
    }

    // Construct the token
    if (!data.empty()) {
        // Convert the name to uppercase so we dont' have to worry about casing.
        for (auto& c : data) {
            c = std::toupper(c);
        }

        outputToken.type = Token::Type::IDENTIFIER;
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

bool Tokenizer::readSymbol(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // Determine if the next character is in the list of symbols.
    for (auto symbol : Tokenizer::SYMBOLS) {
        for (int i = 0; i < symbol.length(); i++) {
            if (peek() == symbol[i]) {
                data += next();
            }
        }
    }

    // Construct the token
    if (!data.empty()) {
        outputToken.type = Token::Type::SYMBOL;
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

bool Tokenizer::readInteger(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // While what we're reading is a digit, add it to the data.
    while (std::isdigit(peek())) {
        data += next();
    }

    // Construct the token.
    if (!data.empty()) {
        outputToken.type = Token::Type::INTEGER;
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

bool Tokenizer::readParens(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // Check for left paranthesis, if found add the token.
    if (peek() == '(') {
        data += next();
        outputToken.type = Token::Type::LPAREN;
    } else if (peek() == ')') {
        // Additionally check for the right paranthesis.
        data += next();
        outputToken.type = Token::Type::RPAREN;
    }

    if (!data.empty()) {
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

bool Tokenizer::readAssignment(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // Check for the assignment operator. This requires both : and = without any whitespace.
    if (peek() == ':') {
        data += next();

        if (peek() == '=') {
            data += next();
        }
    }

    // Construct the token.
    if (data == ":=") {
        outputToken.type = Token::Type::ASSIGNMENT;
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

bool Tokenizer::readOp(Token& outputToken)
{
    std::string data;

    size_t startLine = m_lineNumber;
    size_t startColumn = m_columNumber;

    // Attempt to read an operator.
    if (peek() == '+') {
        data += next();
    } else if (peek() == '-') {
        data += next();
    }

    // Construct the token
    if (!data.empty()) {
        outputToken.type = Token::Type::OP;
        outputToken.data = data;
        outputToken.lineNumber = startLine;
        outputToken.columnNumber = startColumn;
    }

    return !data.empty();
}

void Tokenizer::loadTokens()
{
    Token token;

    // Read al tokens until EOF is found.
    do {
        token = readNextToken();

        // Don't add whitespace to the queue of tokens.
        if (token.type != Token::Type::WHITESPACE) {
            m_tokens.push(token);
        }
    } while (token.type != Token::Type::TEOF);
}

char Tokenizer::next()
{
    // If the index is out of range, return EOF.
    if (m_source.length() <= m_index) {
        return EOF;
    } else {
        char read = m_source[m_index++];

        if (read == '\r') {
            // Technically, carriage return means to reset column number, so handle that here.
            m_columNumber = 0;
        } else if (read == '\n') {
            // Reset the column numner and line number with \n
            // Since on windows it sends \r\n but on linux it's only \n
            m_columNumber = 0;
            m_lineNumber++;
        }

        // Regardless, column number needs incremented.
        m_columNumber++;

        return read;
    }
}

char Tokenizer::peek()
{
    // Check for EOF, and return the current char
    if (m_source.length() <= m_index) {
        return EOF;
    } else {
        return m_source[m_index];
    }
}

void Tokenizer::push()
{
    // Push the current state of the tokenizer.
    TokenizerState state;
    state.columnNumber = m_columNumber;
    state.lineNumber = m_lineNumber;
    state.index = m_index;

    m_states.push(state);
}

void Tokenizer::pop()
{
    // Reset the tokenizer to the last popped state.
    if (!m_states.empty()) {
        TokenizerState state = m_states.top();
        m_states.pop();

        m_columNumber = state.columnNumber;
        m_lineNumber = state.lineNumber;
        m_index = state.index;
    }
}

Token::Token(Token::Type type, const std::string& data, size_t lineNumber, size_t columnNumber)
    : type{type}
    , data{data}
    , lineNumber{lineNumber}
    , columnNumber{columnNumber}
{}
