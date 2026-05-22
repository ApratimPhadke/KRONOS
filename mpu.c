#include "kronos.h"
void mpu_configure_task_region(TCB_t *task){
/*
  MPU->CTRL= 0;
  MPU->RNR = 0;
  MPU->RBAR = (uint32_t)task->stack;
  MPU->RASR = MPU_RASR_ENABLE_Msk | (9 << MPU_RASR_SIZE_Pos) | (3 << MPU_RASR_AP_Pos);
  MPU->RNR = 1;
  MPU->RBAR = 0X00000000;
  MPU->RASR = MPU_RASR_ENABLE_Msk | (31 << MPU_RASR_SIZE_Pos) | (1 << MPU_RASR_AP_Pos);
  MPU->CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
  __DSB();
  __ISB();
*/
}