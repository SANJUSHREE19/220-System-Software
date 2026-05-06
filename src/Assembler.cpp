// Assembler.cpp
#include "Assembler.h"
#include <cctype>

Assembler::Assembler() {
    initialize_opcodes();
}

void Assembler::initialize_opcodes() {
    opcodes = {
        {"LOADI", OP_LOADI}, {"LOAD", OP_LOAD}, {"STORE", OP_STORE}, {"HALT", OP_HALT},
        {"ADDI", OP_ADDI}, {"ADD", OP_ADD}, {"JUMP", OP_JUMP}, {"CMP", OP_CMP}, {"JUMPEQ", OP_JUMPEQ},
        {"PUSH", OP_PUSH}, {"POP", OP_POP}, {"CALL", OP_CALL}, {"RET", OP_RET}, {"MULT", OP_MULT}
    };
}

// Remove trailing commas and surrounding whitespace
static std::string clean_token(std::string s) {
    // trim spaces
    while (!s.empty() && (s.back()==' ' || s.back()=='\t')) s.pop_back();
    while (!s.empty() && (s.front()==' ' || s.front()=='\t')) s.erase(s.begin());
    // drop trailing commas
    while (!s.empty() && (s.back()==',')) s.pop_back();
    return s;
}

// If token is like "[R3]" or "[FP]" (maybe with a trailing comma), return "R3" or "FP"
static std::string strip_brackets(std::string s) {
    s = clean_token(s);
    if (!s.empty() && s.front()=='[') s.erase(s.begin());
    if (!s.empty() && (s.back()==']')) s.pop_back();
    // in case a comma remained after ']' in weird spacing
    while (!s.empty() && (s.back()==',')) s.pop_back();
    return s;
}

uint8_t Assembler::get_reg_index(const std::string& reg_str_in) {
    std::string reg_str = reg_str_in;
    
    // FIX: Safely strip any trailing characters like ','
    while (!reg_str.empty() && !std::isalnum(static_cast<unsigned char>(reg_str.back()))) {
        reg_str.pop_back();
    }
    
    if (reg_str == "SP") return SP;
    if (reg_str == "FP") return FP;
    
    // Original logic
    if (reg_str.size() >= 2 && (reg_str[0] == 'R' || reg_str[0] == 'r')) {
        try { 
            long v = std::stol(reg_str.substr(1), nullptr, 10);
            if (v >= 0 && v < NUM_REGISTERS) return static_cast<uint8_t>(v);
        } catch (...) {
            std::cerr << "Assembler Error: bad register token '" << reg_str_in << "'; expected R0..R5, FP, SP\n";
            std::exit(1);
        }
    }
    // return 0; // Default R0 on error
    std::cerr << "Assembler Error: unknown register '" << reg_str_in << "'\n";
    std::exit(1);
}

int16_t Assembler::clean_and_convert_value(const std::string& value_str_in) {
    std::string value_str = value_str_in;
    
    // 1. Strip all leading/trailing whitespace
    // Find the first non-space character
    value_str.erase(0, value_str.find_first_not_of(" \t\r\n")); 
    // Find the last non-space, non-comma character
    value_str.erase(value_str.find_last_not_of(" \t\r\n,") + 1);

    // 2. Remove '#' prefix (e.g., in #5 or #-1)
    if (value_str.size() > 0 && value_str.front() == '#') {
        value_str.erase(0, 1);
    }
    
    // 3. Handle empty string case
    if (value_str.empty()) return 0; 

    try { 
        // std::stoul is the most robust choice:
        // - It handles decimal numbers (e.g., "5").
        // - It handles negative signs (e.g., "-1").
        // - It handles hex format (e.g., "0x0005" with base=0).
        // The result is cast to int16_t to correctly interpret the two's complement 
        // for negative signed numbers (like -1 converting to 0xFFFF/0xF).
        return (int16_t)std::stol(value_str, nullptr, 0);
    } catch (const std::exception& e) { 
        std::cerr << "Assembler Error: Invalid numeric value '" << value_str_in << "' (cleaned to '" << value_str << "'). Details: " << e.what() << std::endl;
        exit(1); 
    }
}

void Assembler::pass1(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    uint16_t code_addr = CODE_START;
    
    while (std::getline(file, line)) {
        line = line.substr(0, line.find(';')); 
        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token.empty()) continue;

        if (token.back() == ':') { 
            labels[token.substr(0, token.length() - 1)] = code_addr;
        } else if (opcodes.count(token)) {
            code_addr += WORD_SIZE; 
        }
    }
}

void Assembler::pass2(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        line = line.substr(0, line.find(';')); 
        std::stringstream ss(line);
        std::string token;
        ss >> token;

        if (token.empty()) continue;

        // If this line starts with a label, strip it and keep parsing the remainder
        if (!token.empty() && token.back() == ':') {
            // read the next token on the same line (if any)
            ss >> token;
            if (token.empty()) continue; // label-only line
        }

        if (opcodes.count(token)) {
            uint16_t instruction = opcodes[token] << 12; 
            std::string arg1, arg2, arg3;
            char comma;

            if (token == "LOADI") { 
                ss >> arg1 >> arg2;                 // no char comma
                arg1 = clean_token(arg1);           // "R0," -> "R0"
                arg2 = clean_token(arg2);           // "#5," -> "#5"
                uint8_t r_dest = get_reg_index(arg1);
                int16_t raw = clean_and_convert_value(arg2);
                uint8_t imm8 = (uint8_t)(raw & 0xFF);
                instruction |= (r_dest << 8);
                instruction |= imm8;
            }
            else if (token == "STORE") {
                ss >> arg1 >> arg2;
                arg1 = clean_token(arg1);
                arg2 = strip_brackets(arg2);
                uint8_t r_src  = get_reg_index(arg1);
                uint8_t r_addr = get_reg_index(arg2);
                instruction |= (r_src << 8);              // data from r_dest field
                instruction |= (r_addr << 4);
            }
            else if (token == "LOAD") {
                ss >> arg1 >> arg2;
                arg1 = clean_token(arg1);                 // "R0,"
                arg2 = strip_brackets(arg2);              // "[R1]" or "[FP]" -> "R1"/"FP"
                uint8_t r_dest = get_reg_index(arg1);
                uint8_t r_addr = get_reg_index(arg2);
                instruction |= (r_dest << 8);
                instruction |= (r_addr << 4);
            }
            // else if (token == "JUMP" || token == "JUMPEQ") { 
            //     ss >> arg1; 
            //     uint16_t address = labels[arg1]; 
            //     instruction |= address; 
            // }
            else if (token == "ADDI") { 
                ss >> arg1 >> arg2 >> arg3;         // e.g., "R0," "R0," "#-1"
                arg1 = clean_token(arg1);
                arg2 = clean_token(arg2);
                arg3 = clean_token(arg3);
                uint8_t r_dest = get_reg_index(arg1);
                uint8_t r_src1 = get_reg_index(arg2);
                int16_t raw = clean_and_convert_value(arg3);
                uint8_t imm4 = (uint8_t)(raw & 0xF);
                instruction |= (r_dest << 8);
                instruction |= (r_src1 << 4);
                instruction |= imm4; 
            }
            else if (token == "ADD" || token == "MULT") { 
                ss >> arg1 >> arg2 >> arg3;
                arg1 = clean_token(arg1);
                arg2 = clean_token(arg2);
                arg3 = clean_token(arg3);
                uint8_t r_dest = get_reg_index(arg1);
                uint8_t r_src1 = get_reg_index(arg2);
                uint8_t r_src2 = get_reg_index(arg3);
                instruction |= (r_dest << 8);
                instruction |= (r_src1 << 4);
                instruction |=  r_src2;
            }
            else if (token == "CMP") { 
                ss >> arg1 >> arg2;
                arg1 = clean_token(arg1);
                arg2 = clean_token(arg2);
                uint8_t r_lhs = get_reg_index(arg1);
                uint8_t r_rhs = get_reg_index(arg2);
                instruction |= (r_lhs << 8);
                instruction |= (r_rhs << 4);
            }
            else if (token == "PUSH") { // PUSH RSrc (uses R_Src1 field)
                ss >> arg1;
                arg1 = clean_token(arg1);
                uint8_t r_src1 = get_reg_index(arg1);
                instruction |= (r_src1 << 4);
            }
            // POP Rdest
            else if (token == "POP") {
                ss >> arg1;
                arg1 = clean_token(arg1);
                uint8_t r_dest = get_reg_index(arg1);
                instruction |= (r_dest << 8);
            }

            // JUMP/JUMPEQ/CALL label  (J-type)
            else if (token == "JUMP" || token == "JUMPEQ" || token == "CALL") {
                ss >> arg1;
                arg1 = clean_token(arg1);
                uint16_t addr = labels[arg1];
                instruction |= (addr & 0x0FFF);
            }

            else if (token == "RET") {
                // Instruction is just the opcode
            }
            
            machine_code.push_back(instruction);

        }
        else if (token == ".DATA") {
            // parse data items on this line
            std::string data_line;
            std::getline(ss, data_line); // rest of tokens
            std::stringstream data_ss(data_line);
            std::string data_val_str;
            while (std::getline(data_ss, data_val_str, ',')) {
                if (!data_val_str.empty()) {
                    int16_t raw_value = clean_and_convert_value(data_val_str);
                    data_segment.push_back((uint8_t)raw_value);
                }
            }
        }
    }
}

bool Assembler::assemble(const std::string& input_file, Memory& mem) {
    labels.clear();
    machine_code.clear();
    data_segment.clear();

    pass1(input_file);
    pass2(input_file);
    
    if (machine_code.empty() && data_segment.empty()) {
        std::cerr << "Assembly failed or file is empty.\n";
        return false;
    }

    // Load machine code (instructions)
    uint16_t code_addr = CODE_START;
    for (uint16_t instr : machine_code) {
        mem.load_byte(code_addr, (uint8_t)(instr & 0xFF));        
        mem.load_byte(code_addr + 1, (uint8_t)((instr >> 8) & 0xFF)); 
        code_addr += WORD_SIZE;
    }

    // Load data segment (Hello World string)
    uint16_t data_addr = DATA_START;
    for (uint8_t byte : data_segment) {
        mem.load_byte(data_addr++, byte);
    }

    std::cout << "Assembly successful: " << machine_code.size() 
              << " instructions. Data loaded starting at 0x" << std::hex << DATA_START << std::dec << "\n";
    return true;
}