// 头文件
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
/* unix */
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

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

// memory mapped registers
enum
{
    MR_KBSR = 0xFE00,   //keyboard status
    MR_KBDR = 0xFE02    //keyboard data
};

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
    OP_RTI, //(unused), you can ignore this case or throw an error
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

// trap code
// you need it when complete OP_TRAP
enum
{
    TRAP_GETC = 0x20,   //get character from keyboard, not echoed onto the terminal
    // Read a single character from the keyboard. The character is not echoed onto the
    // console. Its ASCII code is copied into R0. The high eight bits of R0 are cleared.
    TRAP_OUT = 0x21,    //output a character
    // Write a character in R0[7:0] to the console display.
    TRAP_PUTS = 0x22,   //output a word string
    // Write a string of ASCII characters to the console display. The characters are contained
    // in consecutive memory locations, one character per memory location, starting with
    // the address specified
    TRAP_IN = 0x23,     //get character from keyboard, echoed onto the terminal
    // Print a prompt on the screen and read a single character from the keyboard. The
    // character is echoed onto the console monitor, and its ASCII code is copied into R0.
    // The high eight bits of R0 are cleared.
    TRAP_PUTSP = 0x24,  //output a byte string
    // Write a string of ASCII characters to the console. The characters are contained in
    // consecutive memory locations, two characters per memory location, starting with the
    // address specified in R0. The ASCII code contained in bits [7:0] of a memory location
    // is written to the console first. Then the ASCII code contained in bits [15:8] of that
    // memory location is written to the console. (A character string consisting of an odd
    // number of characters to be written will have x00 in bits [15:8] of the memory
    // location containing the last character to be written.) Writing terminates with the
    // occurrence of x0000 in a memory location.
    TRAP_HALT = 0x25    //halt the program
    // Halt execution and print a message on the console.
};

// x: srouce number
// bit_count: the bit wide of x
// output: extend x to 16 bit wide by sign extend
uint16_t sign_extend(uint16_t x, int bit_count)
{
    // get sign of x
    // if sign equals '0', return x
    // else, fill 1 for the additional bits
    if ((x >> (bit_count - 1)) & 1)
    {
        x = (0xFFFF << bit_count) | x;
    }
    return x;
}

// Any time a value is written to a register, 
// we need to update the flags to indicate its sign.
void update_flags(uint16_t r)
{
    if(reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    } 
    else if (reg[r] >> 15)
    {
        // sign is 1
        reg[R_COND] = FL_NEG;
    }
    else
    {
        reg[R_COND] = FL_POS;
    }
}

// change endianness
uint16_t swap16(uint16_t x)
{
    return (x << 8) | (x >> 8);
}

// load a program file which containing an array of instructions and data into an address in memory
// the first 16 bits of the program file specify the address in memory where the program should start
// the address is called the origin, it means data start at &memory[origin]
// the address must be read first, the rest of the data will be saved at &memory[origin]
void read_image_file(FILE *file)
{   
    // the address of prograom file in memory
    // read first 16 bits
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    // LC-3 programs are big-endian, 
    // but most of the modern computers we use are little endian.
    origin = swap16(origin);

    // memory start at memory, program data start at memory+origin
    // so the size of remains space is UINT16_MAX-origin
    uint16_t max_read = UINT16_MAX - origin;
    uint16_t *start = memory + origin;
    // the file pointer has been advanced by fread()
    size_t read_count = fread(start, sizeof(uint16_t), max_read, file);

    while (read_count-- > 0)
    {
        // change the endianness of contents
        *start = swap16(*start);
        start++;
    }
} 

// read_image_file from a path
int read_image(const char* image_path)
{
    FILE *file = fopen(image_path, "rb");
    if (!file)
    {
        return 0;
    }
    read_image_file(file);
    fclose(file);
    return 1;
}

// check key
uint16_t check_key()
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

void mem_write(uint16_t address, uint16_t val)
{
    memory[address] = val;
}

uint16_t mem_read(uint16_t address)
{
    // When memory is read from KBSR, 
    // the getter will check the keyboard and update both memory locations.
    if (address == MR_KBSR)
    {
        if (check_key())
        {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = fgetc(stdin);
        }
        else
        {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

// input buffering
struct termios original_tio;

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

// handle interrupt
void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

// main loop
int main(int argc, const char* argv[])
{
    // load augments
    if (argc < 2)
    {
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int i = 1; i < argc; ++i)
    {
        if(!read_image(argv[i]))
        {
            printf("failed to load image: %s\n", argv[i]);
            exit(1);
        }
    }

    //setup
    signal(SIGINT, handle_interrupt);
    disable_input_buffering(); 

    // set condition flag
    // one condition flag should be set at any given time
    reg[R_COND] = FL_ZRO;

    // set the PC to starting position(0x3000)
    enum
    {
        PC_START = 0x3000
    };
    reg[R_PC] = PC_START;

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
            // 
            case OP_ADD:
                /* code */
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    uint16_t sr1 = (instr >> 6) & 0x7;
                    // mode flag
                    uint16_t imm_flag = (instr >> 5) & 0x1;
                    if (!imm_flag)
                    {
                        uint16_t sr2 = instr & 0x7;
                        reg[dr] = reg[sr1] + reg[sr2];
                    }
                    else
                    {
                        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                        reg[dr] = reg[sr1] + imm5;
                    }
                    update_flags(dr);
                }
                break;
            case OP_AND:
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    uint16_t sr1 = (instr >> 6) & 0x7;
                    // mode flag
                    uint16_t imm_flag = (instr >> 5) & 0x1;
                    if (!imm_flag)
                    {
                        uint16_t sr2 = instr & 0x7;
                        reg[dr] = reg[sr1] & reg[sr2];
                    }
                    else
                    {
                        uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                        reg[dr] = reg[sr1] & imm5;
                    }
                    update_flags(dr);
                }
                break;
            case OP_BR:
                {
                    uint16_t n = (instr >> 11) & 0x1;
                    uint16_t z = (instr >> 10) & 0x1;
                    uint16_t p = (instr >> 9) & 0x1;
                    if ((n & (reg[R_COND] == FL_NEG)) || (z & (reg[R_COND] == FL_ZRO)) || (p & reg[R_COND] == FL_POS))
                    {
                        // ++reg[R_PC];i
                        // don't need to increment PC manually, main loop will completed it 
                        reg[R_PC] += sign_extend(instr & 0x1FF, 9);
                    }
                }
                break;
            case OP_JMP:
                {
                    // unconditionally jump
                    uint16_t baseR = (instr >> 6) & 0x7;
                    reg[R_PC] = reg[baseR];
                    // update_flags(R_PC);
                }
                break;
            case OP_JSR:
                {
                    // save incremented PC in R7, address of the next instruction
                    reg[R_R7] = reg[R_PC];
                    uint16_t flag = (instr >> 11) & 0x1;
                    if (!flag)
                    {
                        uint16_t baseR = (instr >> 6) & 0x7;
                        reg[R_PC] = reg[baseR];
                    }
                    else
                    {
                        reg[R_PC] += sign_extend(instr & 0x7FF, 11);
                    }
                    break;
                }
                break;
            case OP_LD:
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    // get address of data
                    uint16_t PCoffset9 = sign_extend(instr & 0x1FF, 9);
                    reg[dr] = mem_read(reg[R_PC] + PCoffset9);
                    update_flags(dr);
                }
                break;
            case OP_LDI:
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    // get address of pointer
                    uint16_t PCoffset = sign_extend(instr & 0x1FF, 9);
                    reg[dr] = mem_read(mem_read(reg[R_PC] + PCoffset));
                    update_flags(dr);
                }
                break;
            case OP_LDR:
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    uint16_t baseR = (instr >> 6) & 0x7;
                    uint16_t Offset6 = sign_extend(instr & 0x3F, 6);
                    reg[dr] = mem_read(reg[baseR] + Offset6);
                    update_flags(dr);
                }
                break;
            case OP_LEA:
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    uint16_t PCoffset9 = sign_extend(instr & 0x1FF, 9);
                    reg[dr] = reg[R_PC] + PCoffset9;
                    update_flags(dr);
                }
                break;
            case OP_NOT:
                {
                    uint16_t dr = (instr >> 9) & 0x7;
                    uint16_t sr = (instr >> 6) & 0x7;
                    reg[dr] = ~reg[sr];
                    update_flags(dr);
                }
                break;
            case OP_ST:
                {
                    // store R[sr] to M[offset]
                    uint16_t sr = (instr >> 9) & 0x7;
                    uint16_t PCoffset9 = sign_extend(instr & 0x1FF, 9);
                    mem_write(reg[R_PC] + PCoffset9, reg[sr]);
                }
                break;
            case OP_STI:
                {
                    // store R[sr] to M[M[offset]]
                    // offset is the address of pointer
                    uint16_t sr = (instr >> 9) & 0x7;
                    uint16_t PCoffset9 = sign_extend(instr & 0x1FF, 9);
                    mem_write(mem_read(reg[R_PC] + PCoffset9), reg[sr]);
                    break;
                }
            case OP_STR:
                {
                    uint16_t sr = (instr >> 9) & 0x7;
                    uint16_t baseR = (instr >> 6) & 0x7;
                    uint16_t Offset6 = sign_extend(instr & 0x3F, 6);
                    mem_write(reg[baseR] + Offset6, reg[sr]);
                    break;
                }
            case OP_TRAP:
                {
                    // reg[R_R7] = reg[R_PC];
                    // the address of system call
                    uint16_t trapvect8 = instr & 0xFF;
                    switch (trapvect8)
                    {
                        case TRAP_GETC:
                            {
                                // get a character, save it into r0
                                uint16_t c = fgetc(stdin);
                                reg[R_R0] = c;
                                update_flags(R_R0);
                                break;
                            }
                        case TRAP_OUT:
                            {
                                // put a character 
                                fputc((char)reg[R_R0], stdout);
                                fflush(stdout);
                                break;
                            }
                        case TRAP_PUTS:
                            {
                                // one character per memory location
                                // string start with the address specified in r0
                                uint16_t *c = memory + reg[R_R0];
                                // 为什么不直接读字符，却要迂回地读地址
                                // 这样是为了使用自增来读取下一个字符
                                while (*c)
                                {
                                    fputc((char)*c, stdout);
                                    c++;
                                }
                                fflush(stdout);
                                break;
                            }
                        case TRAP_IN:
                            {
                                // get a character, show it on terminal, save it into r0
                                printf("enter a character: ");
                                uint16_t c = fgetc(stdin);
                                fputc((char)c, stdout);
                                fflush(stdout);
                                reg[R_R0] = c;
                                update_flags(R_R0);
                                break;
                            }
                        case TRAP_PUTSP:
                            {
                                // two character per memory location
                                uint16_t *c = memory + reg[R_R0];
                                while (*c)
                                {
                                    char c1 = (*c) & 0xFF;
                                    fputc(c1, stdout);
                                    char c2 = (*c) >> 8;
                                    if (c2) fputc(c2, stdout);
                                    c++;
                                }
                                fflush(stdout);
                                break;
                            }
                        case TRAP_HALT:
                            {
                                fputs("HALT!", stdout);
                                fflush(stdout);
                                running = 0;
                                break;
                            }
                        default:
                            break;
                    }
                    break;
                }
            case OP_RES:
            case OP_RTI:
            default:
                abort();
                break;
        }
    }

    // shutdown
    restore_input_buffering();
}