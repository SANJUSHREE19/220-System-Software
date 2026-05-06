; hello_world.asm: Prints "Hello World!" via Memory-Mapped IO

HELLO_MSG: .DATA 0x48,0x65,0x6C,0x6C,0x6F,0x20,0x57,0x6F,0x72,0x6C,0x64,0x21,0x00 ; The string and null terminator

; Code starts at 0x0000

        LOADI R2, #0xFF00               ; R2 = IO_OUTPUT_CHAR_PORT (0xFF00)
        LOADI R3, #0x1000               ; R3 = Address pointer (start of HELLO_MSG)
        LOADI R1, #0                    ; R1 = Null terminator (0) for comparison

LOOP:   
        ; *** NOTE: This program requires a dedicated LOAD instruction (LOAD R0, [R3]) 
        ; *** to be efficient. Since we don't have it, we must rely on a placeholder 
        ; *** assumption that R0 gets the value from memory. For now, this loop
        ; *** will print garbage because we lack the actual LOAD. ***
        
        ; For the sake of demonstrating the loop structure:

        ; JUMP forward to skip the flawed load section for now
        JUMP DUMMY_LOAD
        
DUMMY_LOAD:
        ; Placeholder for: LOADI R0, [R3] (Load the byte R3 points to into R0)
        ; Since we can't load the byte, the execution is incorrect here.
        
        CMP R0, R1                      ; CMP R0, R1 (Compare current char (R0) with null (R1))
        JUMPEQ END                      ; If R0 == R1, jump to END

        STORE R0, [R2]                  ; Write char (R0) to I/O port (R2)

        ADDI R3, R3, #1                 ; R3 = R3 + 1 (Increment pointer)
        JUMP LOOP                       ; Go back to the start of the loop

END:    HALT                            ; Stop the CPU