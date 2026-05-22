/*#include<stdint.h>
#include <stdint.h>

typedef enum IRQn
{
    NonMaskableInt_IRQn   = -14,
    HardFault_IRQn        = -13,
    MemoryManagement_IRQn = -12,
    BusFault_IRQn         = -11,
    UsageFault_IRQn       = -10,
    SVCall_IRQn           = -5,
    DebugMonitor_IRQn     = -4,
    PendSV_IRQn           = -2,
    SysTick_IRQn          = -1,
} IRQn_Type;

#define __CM4_REV                 0x0001
#define __MPU_PRESENT             1
#define __NVIC_PRIO_BITS          4
#define __Vendor_SysTickConfig    0

#include "core_cm4.h"
//defining a task control block
#define MAX_TASK 4
#define TASK_STACK_SIZE 256
typedef enum{
  TASK_READY = 0,
  TASK_RUNNING = 1,
  TASK_BLOCKED = 2,
}TaskState;
typedef struct{
  uint32_t *sp;
  uint32_t stack[TASK_STACK_SIZE];
  TaskState state;
  uint8_t priority;
  uint32_t wake_tick;
  const char *name;
}TCB_t;
//initialising the stack frame for each task 
void kronos_task_init(TCB_t *tcb , void (*task_fn)(void),const char *name){
  tcb->name;
  tcb->state= TASK_READY;
  tcb->priority= 5;
  tcb->wake_tick = 0;
  uint32_t *top = &tcb->stack[TASK_STACK_SIZE-1];
  *top-- = 0x01000000;
  *top-- =(uint32_t)task_fn;
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
  tcb->sp= top+1;
}
//Configure SysTick to tick every 1 ms
#define SYSTICK_LOAD_1MS (16000 - 1)
void systick_init(void){
  SysTick->CTRL=0;
  SysTick->LOAD = SYSTICK_LOAD_1MS;
  SysTick->VAL = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}volatile uint32_t g_tick = 0;
void SysTick_Handler(void){
  g_tick++;
}

int main()
{
  while(1){};
  return 0;
}
*/
  
/*
#include<stdint.h>
#include "kronos.h"
#include<time.h>
extern void task_A(void);
extern void task_B(void);
extern void task_C(void);
extern void task_BAD(void);

void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    DWT->CYCCNT = 0;

    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void idle_task(void)
{
    while(1)
    {
        for(volatile int i = 0; i < 64000; i++);
        SysTick_Handler();
    }
}

void monitor_task(void)
{
    while(1)
    {
        if(g_fault_occured)
        {
            printf("\nFAULT DETECTED\n");

            printf("PC: 0x%08lX\n", g_fault_pc);

            printf("CFSR: 0x%08lX\n", g_fault_cfsr);

            printf("MMFAR: 0x%08lX\n", g_fault_mmfar);

            g_fault_occured = 0;
        }

        sleep_ms(100);
    }
}



int main(){
   //g_tick = 999;
   mpu_init();
   dwt_init();
   NVIC_SetPriority(PendSV_IRQn, 0xFF);
   NVIC_SetPriority(SysTick_IRQn, 0xFE);
   systick_init();

   scheduler_add_task(idle_task, "IDLE");
   scheduler_add_task(task_A, "A");
   scheduler_add_task(task_B, "B");
   scheduler_add_task(task_C, "C");
   scheduler_add_task(task_BAD, "BAD");
   
   g_current_tcb = &g_tasks[0];
   g_next_tcb    = &g_tasks[0];
   start_first_task();
   
     __set_PSP((uint32_t)g_tasks[0].sp);

    __set_CONTROL(0x02);

    __ISB();

   // SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    yield();
    scheduler_add_task(monitor_task, "MON");

    while(1)
    {

    }
  return 0;
}
*/

#include <stdint.h>
#include "kronos.h"

extern void task_A(void);
extern void task_B(void);
extern void task_C(void);


void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/*void idle_task(void)
{
    while(1)
    {
        for(volatile int i = 0; i < 64000; i++);
        SysTick_Handler();
    }
}
*/

void idle_task(void)
{
    g_tick = 42;
    printf("IDLE RUNNING\n");
    while(1)
    {
        for(volatile int i = 0; i < 64000; i++);
        SysTick_Handler();
    }
}

int main(void)
{
    mpu_init();
    dwt_init();
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    NVIC_SetPriority(SysTick_IRQn, 0xFE);
    systick_init();

    scheduler_add_task(idle_task, "IDLE");
    scheduler_add_task(task_A,    "A");
    scheduler_add_task(task_B,    "B");
    scheduler_add_task(task_C,    "C");

    g_current_tcb = &g_tasks[0];
    g_next_tcb    = &g_tasks[0];

    start_first_task();

    while(1){}
}