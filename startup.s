MODULE  startup

        SECTION CSTACK:DATA(3)
        DS8     1024

        SECTION .intvec:CODE(2)
        THUMB
        PUBLIC  __vector_table
        EXTERN  __iar_program_start

__vector_table
        DCD     sfe(CSTACK)
        DCD     __iar_program_start
        DCD     0                   ; NMI
        DCD     0                   ; HardFault
        DCD     0                   ; MemManage
        DCD     0                   ; BusFault
        DCD     0                   ; UsageFault
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     0                   ; SVC
        DCD     0                   ; DebugMon
        DCD     0
        DCD     PendSV_Handler      ; PendSV
        DCD     SysTick_Handler     ; SysTick

        EXTERN  PendSV_Handler
        EXTERN  SysTick_Handler

        SECTION .text:CODE(2)
        THUMB

        PUBLIC  __iar_program_start
__iar_program_start
        LDR     R0, =sfe(CSTACK)
        MSR     MSP, R0
        BL      main
        B       .

        END