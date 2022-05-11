#include "Parser.hpp"
#include "Tokenizer.hpp"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    std::string fileName;

    // Make sure if file argument isn't added, we prompt for one..
    if (argc != 2) {
        std::cout << "Please enter the file name: ";
        std::getline(std::cin, fileName);
    } else {
      // File was supplied as an argument.
      fileName = argv[1];
    }

    Tokenizer tokenizer;

    // Load the specified file.
    if (tokenizer.loadFile(fileName)) {
        std::cout << "Successfully loaded file." << std::endl;

        // Construct a parser for the tokenizer.
        Parser parser{tokenizer};

        // Attempt to parse the file, but if an exception is thrown, report the
        // error to the user.
        try {
            parser.parse();

            // Compilation finished without throwing an exception.
            std::cout << "Successfully compiled " << fileName << "." << std::endl;
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
