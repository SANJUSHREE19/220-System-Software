// Assembler.h
#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "cpu_defs.h"
#include "Memory.h"

class Assembler {
private:
    std::map<std::string, uint16_t> labels;
    std::map<std::string, Opcode> opcodes;
    std::vector<uint16_t> machine_code;
    std::vector<uint8_t> data_segment;
    
    void initialize_opcodes();
    uint8_t get_reg_index(const std::string& reg_str);
    void pass1(const std::string& filename);
    void pass2(const std::string& filename);
    int16_t clean_and_convert_value(const std::string& value_str_in);
    
public:
    Assembler();
    bool assemble(const std::string& input_file, Memory& mem);
};

#endif // ASSEMBLER_H