# CMPE 220 – Software CPU Design Project

## Program Execution and Layout

---

## 📘 Course Information

* **Course:** CMPE 220 – System Software
* **Instructor:** Prof. Ishie Eshwar
* **Semester:** Spring 2026
* **Project:** Software CPU Design – Execution and Layout

---

# 🧠 Project Overview

This project implements a **complete 16-bit Software CPU in C++**, including:

* A working **CPU emulator** (Fetch–Decode–Execute cycle)
* A **memory subsystem** with stack and memory-mapped I/O
* A custom **assembler** that converts assembly code to machine code
* Support for:

  * Arithmetic operations
  * Branching and control flow
  * Function calls and recursion
* Example programs:
  * Factorial (recursive)

---

# ⚙️ How to Compile and Run

## 🔧 Compile

Open terminal in project root:

```bash
g++ -std=c++11 -I./src src/*.cpp -o emulator
```

---

## ▶️ Run Programs
Factorial Program:
```bash
./emulator run asm/factorial.asm
```


Step-through mode (optional):
```bash
./emulator step asm/factorial.asm
```
---

## 🧪 Step Mode (Optional)

```bash
./emulator step asm/factorial.asm
```

Shows:

* Instruction execution
* Register updates
* Program counter changes

---

# 🧾 Expected Output

### Hello World

```text
Hello World!
```

---

### Fibonacci (N = 7)

```text
R4 = 0x000D → 13
```

---

### Factorial (5!)

```text
Final Result: 120 (0x0078)
```

---

# 🏗 CPU Architecture

## 🧮 Registers

| Register | Purpose                |
| -------- | ---------------------- |
| R0–R5    | General purpose        |
| FP (R6)  | Frame Pointer          |
| SP (R7)  | Stack Pointer          |
| PC       | Program Counter        |
| IR       | Instruction Register   |
| Flags    | Zero (Z), Negative (N) |

All registers are **16-bit**.

---

# 📘 Instruction Set Architecture (ISA)

## Instruction Format

```
| OPCODE | RD | RS1 | RS2 / IMM |
```

---

## Supported Instructions

| Instruction | Description        |
| ----------- | ------------------ |
| LOADI       | Load immediate     |
| LOAD        | Load from memory   |
| STORE       | Store to memory    |
| ADD         | Addition           |
| ADDI        | Add immediate      |
| PUSH        | Push to stack      |
| POP         | Pop from stack     |
| CALL        | Function call      |
| RET         | Return             |
| CMP         | Compare            |
| JUMP        | Unconditional jump |
| JUMPEQ      | Jump if zero       |
| MULT        | Multiply           |
| HALT        | Stop execution     |

---

## Flags

* **Z (Zero):** Set if result = 0
* **N (Negative):** Set if result < 0

---

# 🧮 ALU Operations

* 16-bit arithmetic (ADD, ADDI, MULT)
* Logical comparisons (CMP)
* Updates flags (Z, N)

---

# 🗺 Memory Map

```
0xFFFF ─────────────── MMIO (I/O)
0xFF00 ─────────────── Stack (grows downward)
0x1000 ─────────────── Data segment
0x0000 ─────────────── Code segment
```

---

# 🔁 Execution Cycle

Each instruction follows:

1. **Fetch** – Read instruction from memory
2. **Decode** – Identify opcode and operands
3. **Execute** – Perform operation
4. **Store** – Write result back

---

# 📂 Project Structure

```
src/        → CPU + assembler implementation  
asm/        → Assembly programs  
docs/       → diagrams and report  
```

---

# 🧪 Example Programs

---

## 1️⃣ Hello World

Prints characters using **memory-mapped I/O**.

---

## 2️⃣ Fibonacci

Computes Fibonacci sequence iteratively.

```
F(7) = 13
```

Final registers:

```
R4 = 0x000D
```

---

## 3️⃣ Factorial (Recursive)

### Assembly Code

```asm
START:
LOADI R0, #5
LOADI R2, #0
ADD FP, SP, R2
CALL FACTORIAL
ADD R1, R0, R2
HALT

FACTORIAL:
PUSH FP
ADD FP, SP, R2

LOADI R3, #1
LOADI R4, #0

CMP R0, R3
JUMPEQ RETURN_ONE

CMP R0, R4
JUMPEQ RETURN_ONE

PUSH R0
ADDI R0, R0, #-1
CALL FACTORIAL

POP R3
MULT R0, R3, R0
JUMP EPILOGUE

RETURN_ONE:
LOADI R0, #1

EPILOGUE:
ADD SP, FP, R2
POP FP
RET
```

---

## 🔁 Execution Summary

```
R0: 5 → 4 → 3 → 2 → 1  
R0: 1 → 2 → 6 → 18 → 120  
Final: 120 (0x0078)
```

---

# 🧠 Calling Convention

* Argument passed in **R0**
* Return value stored in **R0**
* Stack used for recursion
* FP tracks current frame
* SP manages stack growth

---

# 📊 Stack Frame Layout

```
Higher addresses
-------------------------
| Previous frame        |
| Return PC             |
| Saved FP              |
-------------------------
| Local variables       |
| ... grows downward    |
-------------------------
Lower addresses
```

---

# 👥 Team Contributions

* **Sanjushree Golla**
 CPU architecture, ISA design, core emulator (fetch–decode–execute), integration & debugging

* **Gandhi Soumya Atluri**
  Assembler implementation and instruction encoding

* **Vignesh Jetty Ravi**
  ALU and execution logic

---


# ✅ Conclusion

This project demonstrates:

* CPU design fundamentals
* Instruction execution
* Memory and stack handling
* Assembly to machine code translation
* Recursive function execution

---
