#include "Parser.hpp"
#include "Tokenizer.hpp"

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    // Make sure a file argument is added.
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    Tokenizer tokenizer;

    // Load the specified file.
    if (tokenizer.loadFile(argv[1])) {
        std::cout << "Successfully loaded file." << std::endl;

        // Construct a parser for the tokenizer.
        Parser parser{tokenizer};

        // Attempt to parse the file, but if an exception is thrown, report the error to the user.
        try {
            parser.parse();

            // Compilation finished without throwing an exception.
            std::cout << "Successfully compiled " << argv[1] << "." << std::endl;
        } catch (ParserException& e) {
            std::cout << "Expected " << e.expected() << ", but found "
                      << e.actual() << " at " << e.lineNumber() << ":"
                      << e.columnNumber() << std::endl;
        }
    } else {
        std::cout << "Unable to load file." << std::endl;
    }

    return 0;
}
