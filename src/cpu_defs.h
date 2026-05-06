// cpu_defs.h
#ifndef CPU_DEFS_H
#define CPU_DEFS_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdint> // Required for fixed-width integers (uint16_t, uint8_t)

// --- ARCHITECTURE CONSTANTS ---
#define WORD_SIZE 2         // 16 bits = 2 bytes
#define MEM_SIZE 65536      // 64 KB (0x0000 to 0xFFFF)
#define MAX_CODE_SIZE 4096  // 4 KB for code

// Register Definitions (Indices for the register array)
enum Register {
    R0 = 0, R1, R2, R3, R4, R5, 
    FP, // R6: Frame Pointer
    SP, // R7: Stack Pointer
    NUM_REGISTERS // Should be 8
};

// --- MEMORY MAP CONSTANTS ---
#define CODE_START 0x0000
#define DATA_START 0x1000
#define IO_START 0xFF00
#define IO_OUTPUT_CHAR_PORT 0xFF00 // The memory address we write to for output
#define STACK_START 0xFF00 // Stack grows towards lower addresses

// --- INSTRUCTION SET (Opcodes: bits 15-12) ---
enum Opcode {
    OP_LOADI = 0x1,  // LOADI RDest, Immediate (I-Type)
    OP_STORE = 0x3,  // STORE RDest, [RAddr] (M-Type)
    OP_ADDI  = 0x5,  // ADDI RDest, RSrc, Immediate (I-Type)
    OP_JUMP  = 0x9,  // JUMP Address (J-Type)
    OP_CMP   = 0xA,  // CMP RDest, RSrc (R-Type) - Sets Flags
    OP_JUMPEQ= 0xB,  // JUMPEQ Address (J-Type) - Jumps if Zero Flag is set
    OP_HALT  = 0xF,   // HALT (J-Type/Control)
    OP_PUSH  = 0x6,  // PUSH RSrc (R-Type)
    OP_POP   = 0x7,  // POP RDest (R-Type)
    OP_CALL  = 0x8,  // CALL Address (J-Type)
    OP_RET   = 0xC,  // RET (R-Type)
    OP_LOAD = 0x2,   // NEW: LOAD RDest, [RAddr]
    OP_ADD = 0x4,
    OP_MULT  = 0xD,  // MULT RDest, RSrc1, RSrc2 (R-Type)
};

// --- FLAG CONSTANTS ---
#define FLAG_ZERO (1 << 0) // Bit 0
#define FLAG_CARRY (1 << 1) // Bit 1
#define FLAG_NEGATIVE (1 << 2) // Bit 2
// ... others

#endif // CPU_DEFS_H