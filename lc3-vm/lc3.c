// 头文件
#include <stdint.h>

// 地址空间
// 65536
uint16_t memory[UINT16_MAX];

// registers
// 10个寄存器，8个通用寄存器，1个程序计数器，1个条件标志
// 没个寄存器都是16位大小的
enum
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC,
    R_COND,
    R_COUNT
};
// 用一个数组来存储10个寄存器的内容
uint16_t reg[R_COUNT];

// lc-3 instruction set
enum
{
    OP_BR = 0,  //branch
    OP_ADD, //add
    OP_LD,  //load
    OP_ST,  //store
    OP_JSR, //jump register
    OP_AND, //bitwise and
    OP_LDR, //load register
    OP_STR, //store register
    OP_RTI, //unused, you can ignore this case or throw an error
    OP_NOT, //bitwise not
    OP_LDI, //load indirect, load data from memory
    OP_STI, //store indirect
    OP_JMP, //jump
    OP_RES, //reserved(unused)
    OP_LEA, //load effective address
    OP_TRAP,    //execute trap
};

// condition flags
enum
{
    FL_POS = 1 << 0,
    FL_ZRO = 1 << 1,
    FL_NEG = 1 << 2,
};

// main loop
int main(int argc, const char* argv[])
{
    // load augments
    if (argc < 2)
    {

    };

    //setup

    // set condition flag
    // one condition flag should be set at any given time
    reg[R_COND] = FL_ZRO;

    // set the PC to starting position(0x3000)

    // execute
    int running = 1;
    while (running) 
    {
        // fetch instruction
        uint16_t instr = mem_read(reg[R_PC]++);
        // get operation code
        // operation code at high 4
        uint16_t op = instr >> 12;

        switch (op)
        {
        case OP_ADD:
            /* code */
            break;
        case OP_AND:
            break;
        case OP_BR:
            break;
        case OP_JMP:
            break;
        case OP_JSR:
            break;
        case OP_LD:
            break;
        case OP_LDI:
            break;
        case OP_LDR:
            break;
        case OP_LEA:
            break;
        case OP_NOT:
            break;
        case OP_ST:
            break;
        case OP_STI:
            break;
        case OP_STR:
            break;
        case OP_TRAP:
            break;
        case OP_RES:
        case OP_RTI:
        default:
            break;
        }
    }

    // shutdown
}