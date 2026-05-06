; factorial.asm: Calculates N! recursively (e.g., 5! = 120)
; R0 = N (Argument/Return Value), R1 = Final Result

; --- Data Initialization ---
N_VALUE: .DATA 0x0005 ; N = 5

; --- Main Program ---
START:
        LOADI R0, #5                    ; R0 = N (Argument for factorial)
        LOADI R2, #0                    ; R2 = 0 (Used as a zero-constant register)
        
        ADD FP, SP, R2                  ; FP = SP (Initialize FP to Stack Start)
                                        
        CALL FACTORIAL                  ; Call FACTORIAL(5)
        
        ADD R1, R0, R2                  ; R1 = R0 (Move final result from R0 to R1)
                                        
        HALT                            ; Program finishes, Result is in R1 (120)

; --- Factorial Function ---
FACTORIAL:
        ; 1. FUNCTION PROLOGUE: Set up new stack frame
        PUSH FP                         ; Save old FP onto stack
        ADD FP, SP, R2                  ; FP = SP (R2 is 0, so FP is now current SP)
                                        
        ; 2. BASE CASE CHECK: if N == 1 or N == 0, return 1
        LOADI R3, #1                    ; R3 = 1
        LOADI R4, #0                    ; R4 = 0
        
        CMP R0, R3                      ; CMP N (R0) with 1 (R3)
        JUMPEQ RETURN_ONE               ; If N == 1, jump to base case return

        CMP R0, R4                      ; CMP N (R0) with 0 (R4)
        JUMPEQ RETURN_ONE               ; If N == 0, jump to base case return

        ; 3. RECURSIVE STEP: N * FACTORIAL(N-1)
        
        ; Save R0 (N) for multiplication *after* the recursive call
        PUSH R0                         
        
        ADDI R0, R0, #-1                ; R0 = N - 1 (Prepare argument for next call)
        
        CALL FACTORIAL                  ; RECURSIVE CALL: FACTORIAL(N-1)
                                        ; Result (N-1)! comes back in R0
        
        ; 4. CALC: Restore N, calculate N * (N-1)!
        POP R3                          ; R3 = N (Restore N from stack)
        
        MULT R0, R3, R0                 ; R0 = R3 (N) * R0 ((N-1)!) -> New R0 is N!
        
        JUMP FUNCTION_EPILOGUE
        
RETURN_ONE:
        LOADI R0, #1                    ; Base case: R0 = 1 (Return value)
        
FUNCTION_EPILOGUE:
        ; 5. FUNCTION EPILOGUE: Restore stack and return
        ADD SP, FP, R2                  ; SP = FP (Deallocate local variables/saved registers)
        POP FP                          ; Restore old FP
        RET                             ; Return to caller (Restores PC from stack)