/*typedef enum IRQn
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

#include "core_cm4.h"*/
#include "kronos.h"
#include "core_cm4.h"
#define SYSTICK_LOAD_1MS (64000 - 1)
//#define SYSTICK_LOAD_1MS (SystemCoreClock / 1000 - 1)
volatile uint32_t g_tick =0;

void systick_init(void) {
    SysTick->CTRL = 0; 
    SysTick->LOAD = 63999; 
    
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
                    SysTick_CTRL_TICKINT_Msk | 
                    SysTick_CTRL_ENABLE_Msk;
}

/*
void systick_init(void){
  SysTick->CTRL = 0;
 // SysTick->LOAD = SYSTICK_LOAD_1MS;
  SysTick->LOAD = SYSTICK_LOAD_1MS;
  SysTick->VAL = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk;
}
*/
/*
void SysTick_Handler(void){
  g_tick++;
  scheduler_wake_blocked_tasks(g_tick);
  scheduler_pick_next();
  if (g_next_tcb != g_current_tcb)
    {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}
*/
/*
#pragma vector = SysTick_IRQn;
__interrupt void SysTick_Handler(void)
{
    g_tick++;
    scheduler_wake_blocked_tasks(g_tick);
    scheduler_pick_next();
    if (g_next_tcb != g_current_tcb)
    {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}
*/
void SysTick_Handler(void)
{
    g_tick++;
    scheduler_wake_blocked_tasks(g_tick);
    scheduler_pick_next();
    if (g_next_tcb != g_current_tcb)
    {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}