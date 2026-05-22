       /* EXTERN g_current_tcb
        EXTERN g_next_tcb
        EXTERN ctx_start
        EXTERN ctx_end
        EXTERN ctx_cycles
        EXTERN mpu_configure_task_region

        PUBLIC PendSV_Handler

        SECTION .text:CODE(2)
        THUMB

PendSV_Handler:

        ; =========================
        ; Start timing
        ; =========================

        LDR     R0, =0xE0001004
        LDR     R1, [R0]

        LDR     R2, =ctx_start
        STR     R1, [R2]

        ; =========================
        ; Save current task
        ; =========================

        MRS     R0, PSP
        STMDB   R0!, {R4-R11}

        LDR     R1, =g_current_tcb
        LDR     R1, [R1]
        STR     R0, [R1]

        ; =========================
        ; Switch task
        ; =========================

        LDR     R1, =g_next_tcb
        LDR     R1, [R1]

        LDR     R2, =g_current_tcb
        STR     R1, [R2]
        
        
        PUSH    {LR}

        MOV     R0, R1

        BL      mpu_configure_task_region

        POP     {LR}


        ; =========================
        ; Restore next task
        ; =========================

        LDR     R0, [R1]
        LDMIA   R0!, {R4-R11}

        MSR     PSP, R0

        ; =========================
        ; End timing
        ; =========================

        LDR     R0, =0xE0001004
        LDR     R1, [R0]

        LDR     R2, =ctx_end
        STR     R1, [R2]

        LDR     R2, =ctx_start
        LDR     R3, [R2]

        SUB     R1, R1, R3

        LDR     R2, =ctx_cycles
        STR     R1, [R2]

        ; Return to thread mode using PSP
        LDR     LR, =0xFFFFFFFD

        BX      LR

        END
        */
        /*---------------------------------------------------------------made some changes -----------------------------------------*/
        
        /*
        EXTERN g_current_tcb
        EXTERN g_next_tcb
        EXTERN ctx_start
        EXTERN ctx_end
        EXTERN ctx_cycles
        EXTERN mpu_configure_task_region

        PUBLIC PendSV_Handler

        SECTION .text:CODE(2)
        THUMB

PendSV_Handler:

        ; =====================================
        ; Start context switch timing
        ; =====================================

        LDR     R0, =0xE0001004         ; DWT->CYCCNT
        LDR     R1, [R0]

        LDR     R2, =ctx_start
        STR     R1, [R2]

        ; =====================================
        ; Save current task context
        ; =====================================

        MRS     R0, PSP

        STMDB   R0!, {R4-R11}

        LDR     R1, =g_current_tcb
        LDR     R1, [R1]

        STR     R0, [R1]

        ; =====================================
        ; Switch to next task
        ; =====================================

        LDR     R1, =g_next_tcb
        LDR     R1, [R1]

        LDR     R2, =g_current_tcb
        STR     R1, [R2]

        ; =====================================
        ; Configure MPU for next task
        ; =====================================
        PUSH    {R1, LR}
        
        MOV     R0, R1

        BL      mpu_configure_task_region
        
        POP     {R1, LR}

        ; IMPORTANT:
        ; BL overwrites LR
        ; Restore correct EXC_RETURN manually

        ;LDR     LR, =0xFFFFFFFD
        ;MOVS    R0, #0xFD
        ;MVNS    LR, R0
        MOVS    R0, #0x02
        MVNS    LR, R0

        ; =====================================
        ; Restore next task context
        ; =====================================

        LDR     R0, [R1]

        LDMIA   R0!, {R4-R11}

        MSR     PSP, R0

        ; =====================================
        ; End timing
        ; =====================================

        LDR     R0, =0xE0001004
        LDR     R1, [R0]

        LDR     R2, =ctx_end
        STR     R1, [R2]

        LDR     R2, =ctx_start
        LDR     R3, [R2]

        SUB     R1, R1, R3

        LDR     R2, =ctx_cycles
        STR     R1, [R2]

        ; =====================================
        ; Return to Thread mode using PSP
        ; =====================================

        BX      LR
        
        PUBLIC start_first_task

start_first_task:

        ; Load current TCB
        LDR     R0, =g_current_tcb
        LDR     R0, [R0]

        ; Load saved PSP
        LDR     R0, [R0]

        ; Restore software registers
        LDMIA   R0!, {R4-R11}

        ; Set PSP
        MSR     PSP, R0

        ; Switch Thread mode to PSP
        MOVS    R0, #2
        MSR     CONTROL, R0
        ISB

        ; EXC_RETURN = Thread + PSP
        MOVS    R0, #0x02
        MVNS    LR, R0

        ; Hardware restores remaining frame
        BX      LR
        
        ; Set PSP
        MSR     PSP, R0

        ; Switch to PSP + unprivileged Thread mode
        MOVS    R0, #2
        MSR     CONTROL, R0
        ISB

        ; NOW trigger PendSV to do the first real context switch
        LDR     R0, =0xE000ED04
        LDR     R1, =0x10000000
        STR     R1, [R0]

        ; Loop here — PendSV will take over
        B       .
        
        NOP
        NOP

        END
        
        */
        
        EXTERN g_current_tcb
        EXTERN g_next_tcb
        EXTERN ctx_start
        EXTERN ctx_end
        EXTERN ctx_cycles
        EXTERN mpu_configure_task_region

        PUBLIC PendSV_Handler
        PUBLIC start_first_task

        SECTION .text:CODE(2)
        THUMB

PendSV_Handler:
        LDR     R0, =0xE0001004
        LDR     R1, [R0]
        LDR     R2, =ctx_start
        STR     R1, [R2]

        MRS     R0, PSP
        STMDB   R0!, {R4-R11}
        LDR     R1, =g_current_tcb
        LDR     R1, [R1]
        STR     R0, [R1]

        LDR     R1, =g_next_tcb
        LDR     R1, [R1]
        LDR     R2, =g_current_tcb
        STR     R1, [R2]

        PUSH    {R1, LR}
        MOV     R0, R1
        BL      mpu_configure_task_region
        POP     {R1, LR}

        LDR     R0, [R1]
        LDMIA   R0!, {R4-R11}
        MSR     PSP, R0

        LDR     R0, =0xE0001004
        LDR     R1, [R0]
        LDR     R2, =ctx_end
        STR     R1, [R2]
        LDR     R2, =ctx_start
        LDR     R3, [R2]
        SUB     R1, R1, R3
        LDR     R2, =ctx_cycles
        STR     R1, [R2]

        MOVS    R0, #0x02
        MVNS    LR, R0
        BX      LR

/*
start_first_task:
        LDR     R0, =g_current_tcb
        LDR     R0, [R0]
        LDR     R0, [R0]
        LDMIA   R0!, {R4-R11}
        MSR     PSP, R0
        MOVS    R0, #2
        MSR     CONTROL, R0
        ISB
        LDR     R0, =0xE000ED04
        LDR     R1, =0x10000000
        STR     R1, [R0]
        B       .
*/
/*
start_first_task:
        LDR     R0, =g_current_tcb
        LDR     R0, [R0]
        LDR     R0, [R0]
        LDMIA   R0!, {R4-R11}
        MSR     PSP, R0
        MOVS    R0, #2
        MSR     CONTROL, R0
        ISB
        MOVS    R0, #0x02
        MVNS    LR, R0
        BX      LR
        END
        */
        
start_first_task:
        LDR     R0, =g_current_tcb
        LDR     R0, [R0]
        LDR     R0, [R0]

        ; Restore software registers
        LDMIA   R0!, {R4-R11}

        ; Set PSP to hardware frame
        MSR     PSP, R0

        ; Switch to Thread mode + PSP
        MOVS    R0, #2
        MSR     CONTROL, R0
        ISB

        ; Load PC directly from hardware frame (offset 6 = PC)
        ; PSP now points to R0 of hardware frame
        ; Frame: R0,R1,R2,R3,R12,LR,PC,xPSR = offsets 0,4,8,12,16,20,24,28
        MRS     R0, PSP
        LDR     R0, [R0, #24]   ; Load PC from hardware frame
        BX      R0
        END