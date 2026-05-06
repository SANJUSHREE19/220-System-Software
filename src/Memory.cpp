// Memory.cpp
#include "Memory.h"

Memory::Memory() {
    // Memory array is initialized to zero by the class definition
}

void Memory::handle_io_write(uint16_t address, uint16_t data) {
    if (address == IO_OUTPUT_CHAR_PORT) {
        // Output the character (lower 8 bits of the word)
        std::cout << (char)(data & 0xFF); 
    }
}

uint16_t Memory::read_word(uint16_t address) {
    // Little-endian: low byte at [addr], high byte at [addr+1]
    uint16_t lo = memory_array[address];
    uint16_t hi = memory_array[address + 1];
    return (uint16_t)(lo | (hi << 8));
}

void Memory::write_word(uint16_t address, uint16_t data) {
    // Must match read_word's expectation: write LOW first, then HIGH
    memory_array[address]     = (uint8_t)(data & 0xFF);        // LOW
    memory_array[address + 1] = (uint8_t)((data >> 8) & 0xFF); // HIGH

    if (address >= IO_START) handle_io_write(address, data);
}

void Memory::load_byte(uint16_t address, uint8_t data) {
    memory_array[address] = data;
}

void Memory::dump_memory(uint16_t start_addr, uint16_t count) const {
    std::cout << std::hex << std::setfill('0');
    std::cout << "\n--- Memory Dump (" << std::setw(4) << start_addr << " - " 
              << std::setw(4) << start_addr + count - 1 << " [Hex]) ---\n";
    std::cout << "Addr | 00 01 02 03 04 05 06 07 | ASCII\n";
    std::cout << "------------------------------------------------\n";

    for (uint16_t i = 0; i < count; i += 8) {
        uint16_t addr = start_addr + i;
        if ((uint32_t)addr >= MEM_SIZE) break;

        std::cout << std::setw(4) << addr << " | ";
        std::string ascii_chars = "";
        
        for (int j = 0; j < 8; ++j) {
            if (addr + j < MEM_SIZE) {
                uint8_t byte = memory_array[addr + j];
                std::cout << std::setw(2) << (int)byte << " ";
                ascii_chars += (byte >= 32 && byte <= 126) ? (char)byte : '.';
            } else {
                std::cout << "   ";
            }
        }
        std::cout << "| " << ascii_chars << "\n";
    }
    std::cout << "------------------------------------------------\n" << std::dec;
}