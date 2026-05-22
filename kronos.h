#include<stdio.h>
#include<stdint.h>
#define MAX_TASKS 4
#define TASK_STACK_SIZE 256
#ifndef KRONOS_H
#define KRONOS_H
extern volatile uint32_t g_fault_pc;
extern volatile uint32_t g_fault_cfsr;
extern volatile uint32_t g_fault_mmfar;
extern volatile uint8_t  g_fault_occured;
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
extern volatile uint32_t g_tick;
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
extern TCB_t g_tasks[MAX_TASKS];

extern TCB_t *g_current_tcb;
extern TCB_t *g_next_tcb;

extern uint8_t g_task_count;

void kronos_task_init(TCB_t *tcb,
                      void (*task_fn)(void),
                      const char *name);

void scheduler_add_task(void (*fn)(), const char *name);

void scheduler_pick_next(void);

void yield(void);

void mpu_init(void);

void mpu_configure_task_region(TCB_t *task);

void scheduler_wake_blocked_tasks(uint32_t now);

void systick_init(void);

void stack_paint(TCB_t *tcb);

void start_first_task(void);

void SysTick_Handler(void);

uint32_t stack_hwm(TCB_t *tcb);

void sleep_ms(uint32_t);
#endif