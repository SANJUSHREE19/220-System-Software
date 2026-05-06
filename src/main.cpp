// main.cpp
#include "Cpu.h"
#include "Memory.h"
#include "Assembler.h"

void print_help(const std::string& program_name) {
    std::cout << "Usage:\n";
    std::cout << "  " << program_name << " run <input.asm>\n";
    std::cout << "  " << program_name << " step <input.asm>\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_help(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    std::string filename = argv[2];
    
    Memory mem;
    Assembler assembler;
    
    if (!assembler.assemble(filename, mem)) {
        return 1;
    }
    
    Cpu cpu(mem);

    if (command == "run") {
        cpu.run();
        cpu.print_registers();
        mem.dump_memory(DATA_START, 20); // Show loaded data/string
    } else if (command == "step") {
        std::cout << "--- Starting Step Mode (Hello World) ---\n";
        int instr_count = 0;
        while (!cpu.is_halted()) {
            std::cout << "\n\n--- Instruction #" << ++instr_count << " @ PC: 0x" 
                    << std::hex << cpu.get_PC() << std::dec << " ---\n";
            cpu.print_registers();
            mem.dump_memory(DATA_START, 16); // Show data section
            std::cout << "Press ENTER to execute next instruction (q to quit)...";
            if (std::cin.get() == 'q') break;
            cpu.step();
        }
        cpu.print_registers();
    } else {
        print_help(argv[0]);
        return 1;
    }

    return 0;
}