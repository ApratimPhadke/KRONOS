#include "kronos.h" 
#include "core_cm4.h"
TCB_t g_tasks[MAX_TASKS];

TCB_t *g_current_tcb = 0;
TCB_t *g_next_tcb    = 0;

uint8_t g_task_count = 0;
static uint8_t last_idx = 0;

volatile uint32_t ctx_start;
volatile uint32_t ctx_end;
volatile uint32_t ctx_cycles;
volatile uint32_t g_fault_pc     = 0;
volatile uint32_t g_fault_cfsr   = 0;
volatile uint32_t g_fault_mmfar  = 0;
volatile uint8_t  g_fault_occured = 0;

/* Add a task to scheduler */
void scheduler_add_task(void (*fn)(), const char *name)
{
    //TCB_t *t = &g_tasks[g_task_count++];
    if(g_task_count >= MAX_TASKS)
{
    return;
}

TCB_t *t = &g_tasks[g_task_count++];

    kronos_task_init(t, fn, name);
}

/* Round-robin scheduler */
//void scheduler_pick_next(void)
//{
    /* Find next READY task in circular order */
//    uint8_t curr_idx = g_current_tcb - g_tasks;

 //   for (uint8_t i = 1; i <= g_task_count; i++)
//    {
//        uint8_t idx = (curr_idx + i) % g_task_count;

 //       if (g_tasks[idx].state == TASK_READY)
//        {
 //           g_next_tcb = &g_tasks[idx];
//            return;
//        }
//    }

    /* No READY task found */
//    g_next_tcb = g_current_tcb;
//}

/*
void scheduler_pick_next(void)
{
    TCB_t *best = NULL;

    for (uint8_t i = 0; i < g_task_count; i++)
    {
        if (g_tasks[i].state == TASK_READY)
        {
            if (best == NULL ||
                g_tasks[i].priority < best->priority)
            {
                best = &g_tasks[i];
            }
        }
    }

    g_next_tcb = (best != NULL)
               ? best
               : g_current_tcb;
}
*/
void scheduler_pick_next(void)
{
    TCB_t *best = NULL;

    for(uint8_t offset = 1;
        offset <= g_task_count;
        offset++)
    {
        uint8_t idx =
            (last_idx + offset) % g_task_count;

        if(g_tasks[idx].state != TASK_READY)
            continue;

        if(best == NULL ||
           g_tasks[idx].priority < best->priority)
        {
            best = &g_tasks[idx];
            last_idx = idx;
        }
    }

    g_next_tcb =
        (best != NULL)
        ? best
        : &g_tasks[0];
}
/* Voluntarily give up CPU */
void yield(void)
{
    scheduler_pick_next();

    /* Trigger PendSV interrupt */
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

    /* Flush pipeline */
    __DSB();
    __ISB();
}

void sleep_ms(uint32_t ms){
  g_current_tcb->state= TASK_BLOCKED;
  g_current_tcb->wake_tick = g_tick + ms;
  yield();
}

void scheduler_wake_blocked_tasks(uint32_t now)
{
    for (uint8_t i = 0; i < g_task_count; i++)
    {
        if (g_tasks[i].state == TASK_BLOCKED &&
            g_tasks[i].wake_tick <= now)
        {
            g_tasks[i].state = TASK_READY;
        }
    }
}

/*
__attribute__((naked))
void start_first_task(void)
{
    __asm volatile(
        "LDR R0, =g_current_tcb     \n"
        "LDR R0, [R0]               \n"
        "LDR R0, [R0]               \n"

        
        "LDMIA R0!, {R4-R11}        \n"

       
        "MSR PSP, R0                \n"

        
        "MOVS R0, #2                \n"
        "MSR CONTROL, R0            \n"
        "ISB                        \n"

        
        "LDR LR, =0xFFFFFFFD        \n"

        
        "BX LR                      \n"
    );
}
*/