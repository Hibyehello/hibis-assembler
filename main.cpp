#include <cstring>
#include <iostream>
#include <string>

#include "Command-Line-Parser/parser.h"
#include "Assembler.h"

int main(int argc, const char** argv) {
    ArgParser parser("hibis-assembler", argc, argv, true);
    parser.addOption(FLAG, "source", "s", STR, "source.hib");
    parser.addOptionDesc("source", "Adds a source file to compile, only supports a single file currently");

    parser.addOption(FLAG, "output", "o", STR, "output.txt");
    parser.addOptionDesc("output", "Specify an output file to save the machine code to");

    parser.addOption(FLAG, "logisim", "l");
    parser.addOptionDesc("logisim", "Specified if we should output in Logisim ROM image format");

    std::string source_file;
    std::string output_file = "output.txt";

    bool log_out = false;

    while(parser.canParse()) {
        Option* flag = parser.parseFlags();

        if(strcmp(flag->verbose_name, "source") == 0) {
            source_file = flag->arg_str;
        }
        if(strcmp(flag->verbose_name, "output") == 0) {
            output_file = flag->arg_str;
        }
        if(strcmp(flag->verbose_name, "logisim") == 0) {
            log_out = true;
        }
    }

    if(source_file.empty()) {
        std::cerr << std::endl << "No source file specified! Exiting..." << std::endl;
        return 1;
    }

    Assembler assembler(source_file.c_str(), output_file.c_str());

    if(!assembler.parseFile()) {
        std::cerr << "Failed to parse file!" << std::endl;
        return 1;
    }

    if(!assembler.assemble()) {
        std::cerr << "Failed to assemble code!" << std::endl;
        return 1;
    }

    assembler.createImage(log_out);
}
