; fibonacci.asm: Calculates the Nth Fibonacci number iteratively.
; Target N is hardcoded to 7 (Result should be 13).
; Final result is stored in R4.

; --- Data Initialization ---
N_TARGET: .DATA 0x0007 ; N = 7

; --- Code Starts ---

; 1. Setup
        LOADI R0, #7                    ; R0 = N (Target index: 7)
        LOADI R1, #2                    ; R1 = Counter (Starts at 2)
        LOADI R2, #0                    ; R2 = F_i-2 = 0 (F0)
        LOADI R3, #1                    ; R3 = F_i-1 = 1 (F1)

; --- Check Special Cases (N=0, N=1) ---
        LOADI R5, #1
        LOADI R6, #0
        
        CMP R0, R6                      ; CMP N (R0) with 0 (R6)
        JUMPEQ END_0_1                  ; If N=0, R4=0 (already set by LOADI)
        
        CMP R0, R5                      ; CMP N (R0) with 1 (R5)
        JUMPEQ END_0_1                  ; If N=1, result is 1 (will be in R4 after loop start)

        JUMP CALC_LOOP

END_0_1:
        CMP R0, R6                      ; Check if N=0 again
        JUMPEQ END_PROGRAM              ; If N=0, R4 remains 0.
        
        ADDI R4, R5, #0                 ; If N=1, R4 = 1.
        JUMP END_PROGRAM

; 2. Calculation Loop (Starts for N >= 2)
CALC_LOOP:
        ; Check if Counter (R1) == N (R0)
        CMP R1, R0                      ; CMP Counter, N
        JUMPEQ END_LOOP                 ; If Counter == N, loop is done.

        ; R4 = R2 + R3 (F_i = F_i-2 + F_i-1)
        ADD R4, R2, R3                  ; Perform 16-bit addition (Result in R4)

        ; Shift terms: F_i-2 = F_i-1 (R2 = R3)
        ADD R2, R3, R6                  ; R2 = R3 + 0 (R6=0)
                                        
        ; Shift terms: F_i-1 = F_i (R3 = R4)
        ADD R3, R4, R6                  ; R3 = R4 + 0

        ; Increment Counter
        ADDI R1, R1, #1                 ; R1 = R1 + 1 (using ADDI for single-byte immediate)
        JUMP CALC_LOOP                  ; Repeat loop

; 3. Finalization
END_LOOP:
        ADD R4, R3, R6                  ; Final result is R3 (F_N-1) before the last addition. R4 = F_N.
                                        ; The result is already in R4 from the last ADD before END_LOOP was checked.

END_PROGRAM:
        HALT                            ; Final Result is in R4