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
    bool has_cond;
    bool take_label;
};

static std::map<std::string_view, struct OpcodeInfo> str_to_op {
    {"nop", {OPCODE::NOP, 0, 0, 0, false, false}},
    {"put", {OPCODE::PUT, 1, 1, 0, false, false}},
    {"put.", {OPCODE::PUTI, 1, 1, 16, false, false}},
    {"put^", {OPCODE::PUTU, 1, 2, 16, false, false}},
    {"scrn", {OPCODE::SCRN, 0, 1, 0, false, false}},
    {"scrn.", {OPCODE::SCRNI, 0, 1, 7, false, false}},
    {"xor", {OPCODE::XOR, 1, 2, 0, false, false}},
    {"xor.", {OPCODE::XORI, 1, 2, 16, false, false}},
    {"and", {OPCODE::AND, 1, 2, 0, false, false}},
    {"and.", {OPCODE::ANDI, 1, 2, 16, false, false}},
    {"or", {OPCODE::OR, 1, 2, 0, false, false}},
    {"or.", {OPCODE::ORI, 1, 2, 16, false, false}},
    {"add", {OPCODE::ADD, 1, 2, 0, false, false}},
    {"add.", {OPCODE::ADDI, 1, 2, 16, false, false}},
    {"sub", {OPCODE::SUB, 1, 2, 0, false, false}},
    {"sub.", {OPCODE::SUBI, 1, 2, 16, false, false}},
    {"mul", {OPCODE::MUL, 1, 2, 0, false, false}},
    {"mul.", {OPCODE::MULI, 2, 2, 16, false, false}},
    {"mulw", {OPCODE::MULW, 2, 2, 0, false, false}},
    {"b", {OPCODE::B, 1, 1, 0, true, true}},
    {"b.", {OPCODE::BI, 1, 1, 24, true, true}},
    {"bl", {OPCODE::BL, 1, 1, 0, true, true}},
    {"bl.", {OPCODE::BLI, 1, 1, 24, true, true}},
    {"bg", {OPCODE::BG, 1, 1, 0, true, true}},
    {"bg.", {OPCODE::BGI, 1, 1, 24, true, true}},
    {"ble", {OPCODE::BLE, 1, 1, 0, true, true}},
    {"ble.", {OPCODE::BLEI, 1, 1, 24, true, true}},
    {"bge", {OPCODE::BGE, 1, 1, 0, true, true}},
    {"bge.", {OPCODE::BGEI, 1, 1, 24, true, true}},
    {"be", {OPCODE::BE, 1, 1, 0, true, true}},
    {"be.", {OPCODE::BEI, 1, 1, 24, true, true}},
    {"cmp", {OPCODE::CMP, 1, 2, 0, true, false}},
    {"cmp.", {OPCODE::CMPI, 1, 2, 16, true, false}},
    {"inv", {OPCODE::INV, 1, 1, 0, false, false}},
    {"lsr", {OPCODE::LSR, 1, 2, 5, false, false}},
    {"lsl", {OPCODE::LSL, 1, 2, 5, false, false}},
    {"zero", {OPCODE::ZERO, 1, 0, 0, false, false}},
    {"clr", {OPCODE::CLR, 0, 0, 0, false, false}},
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
