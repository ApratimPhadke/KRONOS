#include <stdio.h>
#include "kronos.h"
#define STACK_PATTERN 0xDEADBEEF
void stack_paint(TCB_t *tcb)
{
    for (int i = 0; i < TASK_STACK_SIZE; i++)
    {
        tcb->stack[i] = STACK_PATTERN;
    }
}

uint32_t stack_hwm(TCB_t *tcb)
{
    uint32_t unused = 0;

    while (unused < TASK_STACK_SIZE &&
           tcb->stack[unused] == STACK_PATTERN)
    {
        unused++;
    }

    return (TASK_STACK_SIZE - unused) * 4;
}
void task_A(void){
  while(1){
    printf("[A] running\n"); 
    //yield();
    sleep_ms(500);
  }
}
void task_B(void){
    while(1) {
     printf("[B] running\n"); 
  //yield();
     sleep_ms(500);
  }
}
void task_C(void){
  while(1){
     printf("[C] running\n"); 
    //yield();
     sleep_ms(500);
  }
}
void task_BAD(void)
{
    printf("[BAD] about to violate memory...\n");

    /* Wait so other tasks run first */
    sleep_ms(500);

    /*
       Deliberately illegal access
       Outside task stack region
    */

   // volatile uint32_t *evil_ptr =
        //(volatile uint32_t *)0x20010000;
    volatile uint32_t *evil_ptr =
    (volatile uint32_t *)0xFFFFFFFF;

    /* This should trigger MemManage fault */
    *evil_ptr = 0xDEADBEEF;

    /*
       Should never execute
    */

    printf("[BAD] you should never see this\n");

    while (1)
    {

    }
}

/*
      void idle_task(void)
{
    while(1)
    {
        // Manually simulate SysTick in IAR simulator
        // since IAR simulator does not support SysTick for this device
        for(volatile int i = 0; i < 64000; i++);
        SysTick_Handler();
    }
}

*/
      
int task_main(void) { 
  systick_init(); scheduler_add_task(task_A, "A");
  scheduler_add_task(task_B, "B");
  scheduler_add_task(task_C, "C"); 
  g_current_tcb = &g_tasks[0]; g_next_tcb = &g_tasks[0];
  __set_PSP((uint32_t)g_tasks[0].sp);
  __set_CONTROL(0x02);
  __ISB();
  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  while(1) {}
}