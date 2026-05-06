// Cpu.h
#ifndef CPU_H
#define CPU_H

#include "cpu_defs.h"
#include "Memory.h"

class Cpu {
private:
    uint16_t registers[NUM_REGISTERS] = {0}; 
    uint16_t PC = CODE_START; 
    uint16_t IR = 0; 
    uint8_t flags = 0; 
    bool halted = false;

    Memory& mem; 

    void set_flag(uint8_t flag_bit, bool value);
    void fetch();
    void decode_execute();
    inline void push(uint16_t value) {
        registers[SP] -= WORD_SIZE;                 // SP goes DOWN first
        mem.write_word(registers[SP], value);       // write at new SP
    }

    inline uint16_t pop() {
        uint16_t value = mem.read_word(registers[SP]); // read at current SP
        registers[SP] += WORD_SIZE;                    // SP goes UP after
        return value;
    }

public:
    Cpu(Memory& memory_unit);
    
    void run();
    void step(); 
    void print_registers() const;
    bool is_halted() const { return halted; }
    uint16_t get_register(Register reg_idx) const { return registers[reg_idx]; }
    uint16_t get_PC() const { return PC; }
};

#endif // CPU_H