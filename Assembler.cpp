#include "Assembler.h"
#include "Instructions.h"
#include <bit>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <ios>
#include <iostream>
#include <string_view>
#include <charconv>
#include <sys/types.h>

Assembler::Assembler(const char* source_file, const char* output_file)
    : m_line(1)
{
    m_source_reader = new std::ifstream(source_file, std::ios::in);
    m_output_writer = new std::ofstream(output_file, std::ios::out);
}

bool Assembler::parseFile() {
    std::string line;
    bool res;

    while(std::getline(*m_source_reader, line)) {
        std::size_t pos = line.find_first_not_of(" \t\n\r\v\f");

        if(pos != std::string::npos) {
            if(line.compare(pos, 2, "//") == 0) continue; // Just a comment
            std::string_view line_view(line);
            line_view.remove_prefix(pos);

            res = tokenizeLine(line_view);

            if(!res) return res;
        }

        m_line++;
    }

    return true;
}

bool Assembler::tokenizeLine(std::string_view line) {
    std::string_view current;

    bool token_set = false;
    int dest_count = 0;
    int source_count = 0;

    Token token {};
    int token_start = 0;

    bool in_quotes = false;

    for(int pos = 0; pos < line.length(); pos++) {
        bool end = pos + 1 == line.length();

        if (line[pos] == '\'') {
                    in_quotes = !in_quotes;
        }

        if(end || (std::isspace((unsigned char)line[pos+1]) && !in_quotes)) {
            current = line.substr(token_start, pos-token_start+1);

            auto it = str_to_op.find(current);

            if(it != str_to_op.end()) {
                token.op_info = it->second;
                token_set = true;
            } else if(current[0] == 'r' && token_set) {
                REGISTER reg = determineRegister(current);

                if(reg == REGISTER::UNSET) {
                    std::cerr << "Invalid register selected! Must in range of 0-15 on line: " << m_line << std::endl;
                    return false;
                }
                if(dest_count < token.op_info.dest_count) {
                    token.op_info.dest_count == 1 ?
                        (token.dest_reg = reg) :
                        (token.dest_regs[dest_count] = reg);

                    dest_count++;
                } else if(source_count < token.op_info.source_count) {
                    if(source_count == 1)
                        token.reg_b = reg;
                    else
                        token.reg_a = reg;

                    source_count++;
                }
            } else if (token_set) {
                uint32_t immediate;
                bool from_char = false;
                std::from_chars_result res{};
                const char* first = "";
                const char* last = "";


                if(!current.empty() && current[0] == '\'') {
                    if(current.length() < 3 || current[2] != '\'') {
                        std::cerr << "Expected `'` to end char statement on line: " << m_line << std::endl;;
                        return false;
                    }
                    from_char = true;
                    immediate = (uint32_t)current[1];
                } else {
                first = current.data();
                last = current.data()+current.size();

                int base = (current.size() > 2 && current[0] == '0' && current[1] == 'x') ? 16 : 10;
                if(base == 16) first += 2;

                res = std::from_chars(first, last, immediate, base);

                }

                if((res.ec == std::errc() && res.ptr == last) || from_char) {
                    if(immediate <= 0xFFFFFF) {
                        token.data_in = immediate;
                        token.has_data = true;
                    } else {
                        std::cerr << "Data passed is too large on line: " << m_line << std::endl;
                        return false;
                    }
                } else {
                    std::cerr << "Error parsing data input on line: " << m_line << std::endl;
                    return false;
                }
            }

            if(!end) {
                while(pos + 1 < line.length() && (std::isspace((unsigned char)line[pos+1])))
                    pos++;

                token_start = pos + 1;
            } else {
                m_tokens.push_back(token);
                return true;
            }
        }
    }

    m_tokens.push_back(token);
    return true;
}

REGISTER Assembler::determineRegister(std::string_view register_token) {
    int reg_num = 0;
    const char* first = register_token.data()+1;
    const char* last = register_token.data()+register_token.size();
    auto reg_num_res = std::from_chars(first, last, reg_num);

    if(reg_num_res.ec == std::errc() && reg_num_res.ptr == last) {
        if(reg_num >= 0 && reg_num <= 15) {
            return (REGISTER)reg_num;
        } else {
            std::cerr << "Invalid register specified r" << reg_num << " on line: " << m_line << std::endl;
        }
    }

    return REGISTER::UNSET;
}

bool Assembler::assemble() {
    for(Token t : m_tokens) {
        int dest = t.op_info.dest_count;
        int src = t.op_info.source_count;

        uint8_t out[4] = {};

        out[0] = (uint8_t)t.op_info.op;

        if(t.has_data) {
            if(std::bit_width(t.data_in) > t.op_info.data_width) {
                std::cerr << "Data value `" << t.data_in << "` too large for opcode" << std::endl;
                return false;
            }
        }

        if(dest == 0 && src == 1) {
            if(t.has_data) {
                out[1] = t.data_in >> 16;
                out[2] = (t.data_in >> 8) & 0x00FF;
                out[3] = t.data_in & 0x0000FF;
            } else {
                out[1] = t.reg_a;
            }
        } else if(dest == 1 && src == 1) {
            if(t.has_data) {
                out[1] = t.dest_reg << 4;
                out[2] = (t.data_in >> 8);
                out[3] = t.data_in & 0x00FF;
            } else {
                out[1] = (t.dest_reg << 4) | t.reg_a;
            }
        } else if(dest == 1 && src == 2) {
            if(t.has_data) {
                out[1] = (t.dest_reg << 4) | t.reg_a;
                out[2] = (t.data_in >> 8);
                out[3] = t.data_in & 0x00FF;
            } else {
                out[1] = (t.dest_reg << 4) | t.reg_a;
                out[3] = t.reg_b;
            }
        } else if(dest == 2 && src == 2) {
            if(t.has_data) {
                out[1] = (t.dest_regs[0] << 4) | t.reg_a;
                out[2] = (t.dest_regs[1] << 4) | ((t.data_in >> 8) & 0x0F);
                out[3] = t.data_in & 0x00FF;
            } else {
                out[1] = (t.dest_regs[0] << 4) | t.reg_a;
                out[2] = t.dest_regs[1] << 4;
                out[3] = t.reg_b;
            }
        } else if(dest == 1 && src == 0) {
            out[1] = (t.dest_reg << 4);
        } else if(dest == 0 && src == 2) {
            if(t.has_data) {
                out[1] = t.reg_a;
                out[2] = (t.data_in >> 8) & 0x00FF;
                out[3] = t.data_in & 0x0000FF;
            } else {
                out[1] = t.reg_a;
                out[3] = t.reg_b;
            }
        }

        m_code.push_back(((uint32_t)out[0] << 24) | ((uint32_t)out[1] << 16) | ((uint32_t)out[2] << 8) | ((uint32_t)out[3]));
    }

    return true;
}

void Assembler::createImage(bool logisim) {
    uint32_t offset = 0;
    uint8_t words = 0;

    (*m_output_writer) << std::setfill('0');

    if(logisim)
        (*m_output_writer) << "v3.0 hex words addressed\n" << std::hex << std::setw(2) << offset << ": ";

    for(uint32_t c : m_code) {
        if(logisim && words == 8) {
            (*m_output_writer) << "\n" << std::hex << std::setw(2) << offset << ": ";
            words = 0;
        }

        (*m_output_writer) << std::hex << std::setw(8) << c;

        if(logisim) {
            if(words == 0 || words <= 7) {
                (*m_output_writer) << " ";
            }
            words++;
            offset++;
        }
    }

    while(logisim && words < 8) {
        words++;
        (*m_output_writer) << std::hex << std::setw(8) << 0 << " ";
    }

    (*m_output_writer) << "\n";

    m_output_writer->close();
}
