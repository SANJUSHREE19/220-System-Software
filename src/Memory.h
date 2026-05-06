// Memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include "cpu_defs.h"

class Memory {
private:
    uint8_t memory_array[MEM_SIZE] = {0}; 
    void handle_io_write(uint16_t address, uint16_t data);

public:
    Memory();
    uint16_t read_word(uint16_t address);
    void write_word(uint16_t address, uint16_t data);
    void load_byte(uint16_t address, uint8_t data);
    void dump_memory(uint16_t start_addr, uint16_t count) const;
};

#endif // MEMORY_H