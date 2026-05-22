# KRONOS — Custom ARM Cortex-M4 RTOS

> Lightweight preemptive RTOS built from scratch using C + ARM Assembly on Cortex-M4 with PendSV context switching, SysTick scheduling, MPU isolation, and DWT performance profiling.

---

# Overview

KRONOS is a custom real-time operating system kernel developed for ARM Cortex-M4 architecture using:

* C
* ARM Assembly
* CMSIS
* IAR Embedded Workbench
* Cortex-M4 simulator

The project implements a complete low-level RTOS scheduler architecture including:

* Preemptive multitasking
* PendSV context switching
* SysTick-based scheduling
* MPU memory isolation
* Task sleep/wakeup
* Stack watermarking
* Fault handling
* DWT cycle profiling
* Cooperative yielding

---

# Project Screenshots

## Initial Bring-Up & Kernel Debugging

### Kernel startup and task initialization

![Kernel Startup](./assets/kernel_startup.png)

---

### SysTick handler debugging

![SysTick Debugging](./assets/systick_debug.png)

---

### Successful SysTick execution

![SysTick Working](./assets/systick_working.png)

---

### Final multitasking execution

![Final RTOS Output](./assets/final_output.png)

---

# Features

## Scheduler

* Round-robin scheduling
* Priority-based task selection
* Task blocking
* Sleep scheduling
* Cooperative yielding

## Context Switching

* PendSV-based switching
* PSP/MSP separation
* Software-saved register restoration
* Hardware exception frame restoration

## Memory Protection

* MPU task isolation
* MemManage fault handling
* Illegal memory access detection

## Timing & Profiling

* DWT CYCCNT profiling
* Context switch cycle measurement
* Runtime tick counting

## Debugging Infrastructure

* Stack painting
* Stack high-watermark detection
* Fault diagnostics
* Task state tracking

---

# Project Architecture

```text
main()
    ↓
systick_init()
    ↓
scheduler_add_task()
    ↓
start_first_task()
    ↓
PendSV triggered
    ↓
PendSV restores first task
    ↓
tasks execute
    ↓
SysTick fires
    ↓
scheduler runs
    ↓
PendSV switches tasks
```

---

# Core Components

| File               | Purpose                            |
| ------------------ | ---------------------------------- |
| `context_switch.s` | PendSV + startup context switching |
| `scheduler.c`      | Scheduler engine                   |
| `systick.c`        | SysTick configuration              |
| `kronos.c`         | Task initialization + MPU          |
| `tasks.c`          | User tasks                         |
| `startup.s`        | Vector table                       |
| `main.c`           | System startup                     |
| `kronos.h`         | Core kernel definitions            |

---

# Software Used

## IAR Embedded Workbench

KRONOS was developed using:

* **IAR Embedded Workbench for ARM**
* Version: **9.70.4**
* Target: **ARM Cortex-M4**
* Simulator Debugging Mode

---

# Installing IAR Embedded Workbench

## Step 1 — Download IAR

Download from the official website:

[IAR Embedded Workbench for ARM](https://www.iar.com/embedded-development-tools/iar-embedded-workbench-for-arm?utm_source=chatgpt.com)

Install:

* Embedded Workbench for ARM
* CMSIS support
* Cortex-M simulator components

---

## Step 2 — Create Workspace

Open:

```text
IAR Embedded Workbench
```

Then:

```text
Project → Open Existing Workspace
```

Open:

```text
kronos.eww
```

(or open `kronos.ewp` if workspace missing)

---

# Repository Structure

```text
KRONOS/
│
├── context_switch.s
├── scheduler.c
├── systick.c
├── tasks.c
├── kronos.c
├── mpu.c
├── startup.s
├── main.c
├── kronos.h
│
├── Debug/
├── settings/
│
├── kronos.ewp
├── kronos.eww
├── kronos.ewd
└── README.md
```

---

# How To Build

## Build the project

Inside IAR:

```text
Project → Rebuild All
```

Expected output:

```text
Build succeeded
```

---

# How To Run

## Run in Simulator

Inside IAR:

```text
Project → Download and Debug
```

Then press:

```text
Run (Green Play Button)
```

---

# Expected Output

Terminal I/O should show:

```text
IDLE RUNNING
[A] running
[B] running
[C] running
```

You should also observe:

* `g_tick` incrementing
* PendSV switching tasks
* blocked tasks waking up
* context switching activity

---

# Important Debug Variables

| Variable          | Purpose               |
| ----------------- | --------------------- |
| `g_tick`          | System tick counter   |
| `ctx_cycles`      | Context switch timing |
| `g_current_tcb`   | Current task          |
| `g_next_tcb`      | Next scheduled task   |
| `g_tasks[]`       | Task states           |
| `g_fault_occured` | MPU fault detection   |

---

# Task Control Block

```c
typedef struct{
  uint32_t *sp;
  uint32_t stack[TASK_STACK_SIZE];
  TaskState state;
  uint8_t priority;
  uint32_t wake_tick;
  const char *name;
}TCB_t;
```

---

# SysTick Scheduling

KRONOS uses SysTick interrupts for:

* task timing
* wakeup scheduling
* PendSV triggering

```c
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
```

---

# PendSV Context Switching

Context switching is implemented fully in ARM assembly.

Core operations:

* save current task registers
* switch TCBs
* restore next task
* return to thread mode

```asm
MRS     R0, PSP
STMDB   R0!, {R4-R11}
```

Restore:

```asm
LDMIA   R0!, {R4-R11}
MSR     PSP, R0
```

---

# MPU Memory Isolation

KRONOS includes MPU support for:

* task stack isolation
* invalid memory access detection
* MemManage fault handling

Fault enable:

```c
SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
```

---

# DWT Performance Profiling

The kernel measures:

* context switch timing
* runtime cycles

using:

```c
DWT->CYCCNT
```

This enables precise RTOS profiling.

---

# Stack Diagnostics

KRONOS implements:

* stack painting
* stack high-watermark analysis

Pattern:

```c
#define STACK_PATTERN 0xDEADBEEF
```

Used to detect:

* stack overflow
* stack usage
* memory corruption

---

# Fault Injection Testing

The project includes a deliberately bad task:

```c
task_BAD()
```

which intentionally accesses invalid memory to verify:

* MPU behavior
* fault handling
* scheduler recovery

---

# Major Debugging Challenges Solved

## 1. SysTick never incremented

Problem:

```text
g_tick = 0 forever
```

Root cause:

* missing vector table
* simulator configuration issues

Fix:

* custom startup vector table
* proper SysTick handler wiring

---

## 2. EXC_RETURN failures

Problem:

* CPU stuck after `BX LR`

Fix:

* redesigned `start_first_task`
* PendSV-based first context switch

---

## 3. PendSV LR corruption

Problem:

```asm
BL mpu_configure_task_region
```

destroyed EXC_RETURN.

Fix:

```asm
PUSH {R1, LR}
...
POP {R1, LR}
```

---

## 4. Clock mismatch

Problem:

* incorrect SysTick frequency

Fix:

```c
#define SYSTICK_LOAD_1MS (64000 - 1)
```

for 64 MHz simulator operation.

---

# What Makes KRONOS Different

Unlike typical educational RTOS demos, KRONOS implements:

* Manual Cortex-M exception frame construction
* Real PendSV assembly switching
* DWT hardware profiling
* MPU fault handling
* Stack watermarking
* Tick-based sleep scheduler
* ARM PSP/MSP separation
* Low-level Cortex-M exception return logic

This project focuses heavily on:

* internal RTOS architecture
* Cortex-M internals
* scheduler mechanics
* assembly-level debugging

rather than relying on existing RTOS frameworks.

---

# Future Improvements

Planned improvements:

* priority inheritance
* mutexes/semaphores
* dynamic task creation
* heap allocator
* UART drivers
* hardware board support
* real STM32 deployment
* tickless idle mode

---

# Learning Outcomes

This project required deep understanding of:

* ARM Cortex-M exception model
* PSP/MSP stack behavior
* PendSV scheduling
* SysTick interrupts
* MPU configuration
* assembly-level debugging
* vector tables
* RTOS scheduler architecture

---

# Author

**Apratim**
Electronics & Telecommunication Engineer
Embedded Systems | RTOS | ARM Cortex-M | Low-Level Systems Programming
