#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>

struct TokenizerState
{
    size_t index;
    size_t lineNumber;
    size_t columnNumber;
};

struct Token
{
    enum class Type
    {
        IDENTIFIER,
        KEYWORD,
        INTEGER,
        WHITESPACE,
        SYMBOL,
        LPAREN,
        RPAREN,
        OP,
        ASSIGNMENT,
        UNKNOWN,
        TEOF,
    };

    static std::map<Type, std::string> TYPE_NAMES;

    Token(Type type, const std::string& data, size_t lineNumber, size_t columnNumber);
    Token()
    {}

    Type type;
    std::string data;
    size_t lineNumber;
    size_t columnNumber;
};

class Tokenizer
{
public:
    static std::vector<std::string> KEYWORDS;
    static std::vector<std::string> SYMBOLS;

    Tokenizer();

    /**
     * @brief loadFile loads the contents of the specified file into the {@code CharBuffer}.
     * @param fileName the file name to load
     * @return true if the file exists and was successfully loaded
     */
    bool loadFile(const std::string& fileName);

    /**
     * @brief nextToken retrieves the next {@code Token} from the tokenizer and advances.
     * @return the next {@code Token}
     */
    Token nextToken();

    /**
     * @brief peekToken retrieves the next {@code Token} from the tokenizer, but doesn't advance.
     * @return the next {@code Token}
     */
    Token peekToken();

    /**
     * @brief hasMoreTokens returns whether or not more Tokens exist.
     * @return whether or not more tokens exist
     */
    bool hasMoreTokens()
    {
        return !m_tokens.empty();
    }

protected:
    bool readWhitespace(Token& outputToken);
    bool readIdentifier(Token& outputToken);
    bool readSymbol(Token& outputToken);
    bool readInteger(Token& outputToken);
    bool readParens(Token& outputToken);
    bool readAssignment(Token& outputToken);
    bool readOp(Token& outputToken);

private:
    /**
     * @brief loadTokens loads all of the tokens from the source.
     */
    void loadTokens();

    /**
     * @brief next retrieves the next {@code Token} from the tokenizer.
     * @return the next {@code Token}
     */
    Token readNextToken();

    /**
     * @brief next read and return the next character in the source.
     * If we are at the end of the source code, EOF is returned.
     * The reading index is incremented.
     *
     * @return the next character read
     */
    char next();

    /**
     * @brief peek read and return the next character in the source.
     * If the next character is passed the end of the source code, EOF is
     * returned. The reading index is NOT incremented.
     *
     * @return the next character read
     */
    char peek();

    /**
     * @brief push stores the current source index on the stack.
     */
    void push();

    /**
     * @brief pop returns the source to the last location of push.
     * If no history exists, this is a no-op.
     */
    void pop();

private:
    std::string m_source;

    size_t m_index;
    std::stack<TokenizerState> m_states;

    std::queue<Token> m_tokens;

    unsigned int m_lineNumber;
    unsigned int m_columNumber;

    static Token EOFToken;
};

#endif // TOKENIZER_HPP
