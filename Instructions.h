#pragma once

#include <map>
#include <string_view>

enum OPCODE {
    NOP,
    PUT,
    PUTI,
    PUTU,
    SCRN,
    SCRNI,
    XOR,
    XORI,
    AND,
    ANDI,
    OR,
    ORI,
    ADD,
    ADDI,
    SUB,
    SUBI,
    MUL,
    MULI,
    MULW,
    B,
    BI,
    BL,
    BLI,
    BG,
    BGI,
    BLE,
    BLEI,
    BGE,
    BGEI,
    BE,
    BEI,
    CMP,
    CMPI,
    INV,
    LSR,
    LSL,
    ZERO,
    CLR,
};

enum REGISTER {
    UNSET = -1,
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
};

struct OpcodeInfo {
    OPCODE op;
    int dest_count;
    int source_count;
    int data_width;
};

static std::map<std::string_view, struct OpcodeInfo> str_to_op {
    {"nop", {OPCODE::NOP, 0, 0, 0}},
    {"put", {OPCODE::PUT, 1, 1, 0}},
    {"put.", {OPCODE::PUTI, 1, 1, 16}},
    {"put^", {OPCODE::PUTU, 1, 2, 16}},
    {"scrn", {OPCODE::SCRN, 0, 1, 0}},
    {"scrn.", {OPCODE::SCRNI, 0, 1, 7}},
    {"xor", {OPCODE::XOR, 1, 2, 0}},
    {"xor.", {OPCODE::XORI, 1, 2, 16}},
    {"and", {OPCODE::AND, 1, 2, 0}},
    {"and.", {OPCODE::ANDI, 1, 2, 16}},
    {"or", {OPCODE::OR, 1, 2, 0}},
    {"or.", {OPCODE::ORI, 1, 2, 16}},
    {"add", {OPCODE::ADD, 1, 2, 0}},
    {"add.", {OPCODE::ADDI, 1, 2, 16}},
    {"sub", {OPCODE::SUB, 1, 2, 0}},
    {"sub.", {OPCODE::SUBI, 1, 2, 16}},
    {"mul", {OPCODE::MUL, 1, 2, 0}},
    {"mul.", {OPCODE::MULI, 2, 2, 16}},
    {"mulw", {OPCODE::MULW, 2, 2, 0}},
    {"b", {OPCODE::B, 0, 1, 0}},
    {"b.", {OPCODE::BI, 0, 1, 24}},
    {"bl", {OPCODE::BL, 0, 1, 0}},
    {"bl.", {OPCODE::BLI, 0, 1, 24}},
    {"bg", {OPCODE::BG, 0, 1, 0}},
    {"bg.", {OPCODE::BGI, 0, 1, 24}},
    {"ble", {OPCODE::BLE, 0, 1, 0}},
    {"ble.", {OPCODE::BLEI, 0, 1, 24}},
    {"bge", {OPCODE::BGE, 0, 1, 0}},
    {"bge.", {OPCODE::BGEI, 0, 1, 24}},
    {"be", {OPCODE::BE, 0, 1, 0}},
    {"be", {OPCODE::BEI, 0, 1, 24}},
    {"cmp", {OPCODE::CMP, 0, 2, 0}},
    {"cmp.", {OPCODE::CMPI, 0, 2, 16}},
    {"inv", {OPCODE::INV, 1, 1, 0}},
    {"lsr", {OPCODE::LSR, 1, 2, 5}},
    {"lsl", {OPCODE::LSL, 1, 2, 5}},
    {"zero", {OPCODE::ZERO, 1, 0, 0}},
    {"clr", {OPCODE::CLR, 0, 0, 0}},
};

struct Token {
    OpcodeInfo op_info;

    union {
        REGISTER dest_regs[2];
        REGISTER dest_reg;
    };

    REGISTER reg_a;
    union {
        REGISTER reg_b;
        uint32_t data_in;
    };
    bool has_data;
};
