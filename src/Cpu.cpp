// Cpu.cpp
#include "Cpu.h"

Cpu::Cpu(Memory& memory_unit) : mem(memory_unit) {
    registers[SP] = STACK_START; 
    registers[FP] = STACK_START;
}

void Cpu::set_flag(uint8_t flag_bit, bool value) {
    if (value) flags |= flag_bit;
    else flags &= ~flag_bit;
}

void Cpu::fetch() {
    if (PC < MEM_SIZE - 1 && PC >= CODE_START) {
        IR = mem.read_word(PC);
        PC += WORD_SIZE; 
    } else {
        halted = true;
    }
}

void Cpu::decode_execute() {
    uint8_t opcode = (IR >> 12) & 0xF;
    uint8_t r_dest_idx = (IR >> 8) & 0xF;
    uint8_t r_src1_idx = (IR >> 4) & 0xF;
    uint8_t r_src2_idx = IR & 0xF;
    uint8_t immediate_8bit = IR & 0xFF; 
    int16_t signed_immediate_4bit;
    if (IR & 0x0008) { // Check if the sign bit (bit 3) is set
        // Sign-extend to 16 bits (0xF...F)
        signed_immediate_4bit = (IR & 0x000F) | 0xFFF0; 
    } else {
        // Zero-extend (positive number)
        signed_immediate_4bit = IR & 0x000F;
    }
    uint16_t address_12bit = IR & 0xFFF;
    

    // Check for valid register indices (though not strictly necessary for this simple ISA)
    if (r_dest_idx >= NUM_REGISTERS || r_src1_idx >= NUM_REGISTERS || r_src2_idx >= NUM_REGISTERS) {
        // If execution uses an invalid index, it will access outside the array, causing an error.
    }

    auto fmt16 = [](uint16_t v) {
        std::ostringstream o;
        o << "0x" << std::uppercase << std::hex
        << std::setw(4) << std::setfill('0') << v;
        return o.str();
    };


    // ===== BEGIN ultra-light trace (remove after debugging) =====
        auto trace = [&](const char* mnem){
            std::cout << "[PC=" << fmt16(PC - WORD_SIZE)
                    << "] " << mnem
                    << " | R0=" << fmt16(registers[R0])
                    << " R1=" << fmt16(registers[R1])
                    << " SP=" << fmt16(registers[SP])
                    << " FP=" << fmt16(registers[FP])
                    << " ZN=" << ((flags & FLAG_ZERO) ? '1' : '0')
                                << ((flags & FLAG_NEGATIVE) ? '1' : '0')
                    << "\n";
        };
        // ===== END ultra-light trace =====
    
    switch (opcode) {

        case OP_LOADI: { // LOADI RDest, #Immediate (8-bit)
            trace("LOADI");
            registers[r_dest_idx] = immediate_8bit;
            break;
        }
        case OP_LOAD: { // LOAD RDest, [RAddr] (RAddr is R_Src1)
            trace("LOAD");
            uint16_t address = registers[r_src1_idx]; // R_Src1 holds the memory address
            registers[r_dest_idx] = mem.read_word(address);
            break;
        }
        case OP_STORE: { // STORE RDest, [RAddr]
            trace("STORE");
            uint16_t store_data = registers[r_dest_idx];
            uint16_t address = registers[r_src1_idx]; // R_Src1 holds the memory address
            mem.write_word(address, store_data);
            break;
        }
       case OP_ADDI: { // ADDI RDest, RSrc, #imm4  (imm4 is SIGNED: -8..+7)
            trace("ADDI");
            // sign-extend low 4 bits of IR to 16-bit
            int16_t imm4 = (IR & 0x0008) ? (int16_t)((IR & 0x000F) | 0xFFF0)
                                        : (int16_t)(IR & 0x000F);

            // do the math in a wider type, then truncate back to 16 bits
            int32_t tmp = (int32_t)registers[r_src1_idx] + (int32_t)imm4;
            registers[r_dest_idx] = (uint16_t)(tmp & 0xFFFF);

            set_flag(FLAG_ZERO, registers[r_dest_idx] == 0);
            set_flag(FLAG_NEGATIVE, (registers[r_dest_idx] & 0x8000) != 0);
            break;
        }
        case OP_JUMP: { // JUMP Address (12-bit)
            trace("JUMP");
            PC = address_12bit;
            break;
        }
        case OP_CMP: { // CMP RDest, RSrc
            trace("CMP");
            int16_t a = static_cast<int16_t>(registers[r_dest_idx]);
            int16_t b = static_cast<int16_t>(registers[r_src1_idx]);
            set_flag(FLAG_ZERO, a == b);
            set_flag(FLAG_NEGATIVE, a < b);
            break;
        }
        case OP_JUMPEQ: { // JUMPEQ Address (12-bit)
            trace("JUMPEQ");
            if (flags & FLAG_ZERO) {
                PC = address_12bit;
            }
            break;
        }
        case OP_PUSH: { // PUSH RSrc (R-Type: uses R_Src1 as the source register)
            trace("PUSH");
            uint16_t value = registers[r_src1_idx];
            push(value);
            break;
        }
        case OP_POP: { // POP RDest (R-Type: uses R_Dest as the destination register)
            trace("POP");
            registers[r_dest_idx] = pop();
            break;
        }
        case OP_CALL: { // CALL Address (J-Type: saves PC, jumps to address)
            // 1. Save Return Address (PC is already advanced to the instruction after CALL)
            trace("CALL");
            push(PC); 
            // 2. Jump to new address
            PC = address_12bit;
            break;
        }
        case OP_RET: { // RET (R-Type: pops PC from stack)
            trace("RET");
            // 1. Restore Return Address
            PC = pop();
            break;
        }
        case OP_HALT: {
            trace("HALT");
            halted = true;
            break;
        }
        case OP_ADD: { // ADD RDest, RSrc1, RSrc2
            trace("ADD");
            uint16_t result = registers[r_src1_idx] + registers[r_src2_idx];
            registers[r_dest_idx] = result;
            set_flag(FLAG_ZERO, result == 0);
            break;
        }
        case OP_MULT: { // MULT RDest, RSrc1, RSrc2
            trace("MULT");
            // Since we use 16-bit registers (uint16_t), the result may overflow.
            // We cast the multiplication to store the lower 16 bits in the destination register.
            uint32_t long_result = (uint32_t)registers[r_src1_idx] * registers[r_src2_idx];
            
            // Store the lower 16 bits
            registers[r_dest_idx] = (uint16_t)(long_result & 0xFFFF);
            
            // The result is stored, check for Zero flag.
            set_flag(FLAG_ZERO, registers[r_dest_idx] == 0);
            
            // NOTE: A real CPU would likely also set an OVERFLOW flag if long_result > 0xFFFF
            // We omit that check for this project's simplicity.
            break;
        }
        default:
            std::cerr << "ERROR: Unknown opcode 0x" << std::hex << (int)opcode << " at PC 0x" << PC - WORD_SIZE << "\n";
            halted = true;
            break;
    }
}

void Cpu::run() {
    // --- TEMP sanity for stack round-trip ---
push(0x1234);
uint16_t t = pop();
if (t != 0x1234) {
    std::cerr << "Stack round-trip FAILED: wrote 0x1234, popped 0x"
              << std::hex << t << std::dec << "\n";
    halted = true;  // bail early; fix Memory::write_word ordering
    return;
}
// --- END TEMP ---
    std::cout << "--- CPU Running ---\n";
    while (!halted) {
        fetch();
        if (!halted) decode_execute();
    }
    std::cout << "\n--- CPU Halted ---\n";
}

void Cpu::step() {
    if (!halted) {
        fetch();
        if (!halted) decode_execute();
    }
} 

void Cpu::print_registers() const {
    std::cout << std::hex << std::setfill('0');
    std::cout << "\n--- Registers ---\n";
    std::cout << "PC: 0x" << std::setw(4) << PC << " | IR: 0x" << std::setw(4) << IR << " | Flags: 0x" << std::setw(2) << (int)flags << "\n";
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        std::string name = (i == SP) ? "SP" : (i == FP) ? "FP" : "R" + std::to_string(i);
        std::cout << name << ": 0x" << std::setw(4) << registers[i];
        if ((i + 1) % 4 == 0) std::cout << "\n";
        else std::cout << " | ";
    }
    std::cout << std::dec << "\n";
}