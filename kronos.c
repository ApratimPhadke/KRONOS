#include "kronos.h"

void mpu_init(void){
  MPU->CTRL = 0;
  MPU->RNR = 0;
  MPU->RBAR = 0X20000000;
  MPU->RASR = (0X03 << 24) | (14 << 1) | (1 << 0);
  MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
  __DSB();
  __ISB();
}

/*
void MemManage_Handler(void)
{
   
    uint32_t cfsr  = SCB->CFSR;

    uint32_t mmfar = SCB->MMFAR;

    uint32_t *stacked_frame =
        (uint32_t *)__get_PSP();

   
    uint32_t fault_pc =
        stacked_frame[6];

    printf("\n[KRONOS FAULT]\n");

    printf(" Task : %s\n",
           g_current_tcb->name);

    printf(" CFSR : 0x%08X\n",
           cfsr);

    printf(" Fault PC : 0x%08X\n",
           fault_pc);

    
    if (cfsr & 0x80)
    {
        printf(" MMFAR : 0x%08X\n",
               mmfar);
    }

    g_current_tcb->state =
        TASK_BLOCKED;

    g_current_tcb->wake_tick =
        UINT32_MAX;

    yield();
}

*/

void MemManage_Handler(void)
{
    uint32_t *stacked_frame =
        (uint32_t *)__get_PSP();

    g_fault_pc    = stacked_frame[6];
    g_fault_cfsr  = SCB->CFSR;
    g_fault_mmfar = SCB->MMFAR;

    g_fault_occured = 1;

    g_current_tcb->state = TASK_BLOCKED;

    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

    while(1)
    {
        __NOP();
    }
}

/*
void kronos_task_init(TCB_t *tcb,void(*task_fn)(void),const char *name){
  tcb->name = name;
  tcb->state = TASK_READY;
  tcb->priority = 5;
  tcb->wake_tick = 0;
  stack_paint(tcb);
  uint32_t *top = &tcb->stack[TASK_STACK_SIZE -1];
  top = (uint32_t *)((uint32_t)top & ~0x7);
  *top-- = 0x01000000;
  *top-- = (uint32_t)task_fn;
  *top-- = 0xFFFFFFFD;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  *top-- = 0x00000000;
  tcb->sp = top + 1;
}
*/

void kronos_task_init(TCB_t *tcb,
                      void (*task_fn)(void),
                      const char *name)
{
    stack_paint(tcb);

    tcb->state = TASK_READY;
    tcb->priority = 1;
    tcb->wake_tick = 0;
    tcb->name = name;

    uint32_t *top =
        &tcb->stack[TASK_STACK_SIZE];

    /*
       8-byte alignment
    */
    top = (uint32_t *)
          ((uint32_t)top & ~0x7);

    /*
       Hardware stack frame
       (AUTO-RESTORED by Cortex-M)
    */

    *(--top) = 0x01000000;          // xPSR
    *(--top) = (uint32_t)task_fn;   // PC
    *(--top) = 0xFFFFFFFD;          // LR
    *(--top) = 0x12121212;          // R12
    *(--top) = 0x03030303;          // R3
    *(--top) = 0x02020202;          // R2
    *(--top) = 0x01010101;          // R1
    *(--top) = 0x00000000;          // R0

    /*
       Software-saved registers
       (RESTORED by PendSV)
    */

    *(--top) = 0x11111111;          // R11
    *(--top) = 0x10101010;          // R10
    *(--top) = 0x09090909;          // R9
    *(--top) = 0x08080808;          // R8
    *(--top) = 0x07070707;          // R7
    *(--top) = 0x06060606;          // R6
    *(--top) = 0x05050505;          // R5
    *(--top) = 0x04040404;          // R4

    /*
       PSP points to software frame
    */

    tcb->sp = top;
}