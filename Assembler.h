#pragma once

#include "Instructions.h"
#include <cstdint>
#include <fstream>
#include <list>
#include <string_view>

class Assembler {
public:
    Assembler(const char* source_file, const char* output_file);

    bool parseFile();
    bool assemble();
    void createImage(bool logisim);

    void printCode() {
        uint32_t offset = 0;
        uint8_t words = 0;
        printf("v3.0 hex words addressed\n");
        printf("%02x:", offset);
        for(uint32_t c : m_code) {
            printf(" %08x", c);
            words++;
            if(words == 8) {
                printf("\n%02x: ", offset);
                offset += 8;
                words = 0;
            }
        }
        while(words != 8 && words != 0) {
            printf(" %08x", 0);
            words++;
        }
        printf("\n");
    }

private:
    bool tokenizeLine(std::string_view line);
    REGISTER determineRegister(std::string_view register_token);

private:
    std::ofstream* m_output_writer;
    std::ifstream* m_source_reader;

    std::list<Token> m_tokens;
    int m_line;

    std::list<uint32_t> m_code;
};
