#include "FreeRTOS.h"
#include "setup_osal.h"

#include <stdbool.h>

_Bool __atomic_is_lock_free(unsigned int size, const volatile void *ptr)
{
    (void)size;
    (void)ptr;

    return true;
}

unsigned int __atomic_fetch_add_4(volatile void *ptr, unsigned int val, int memorder)
{
    (void)(memorder);

    int tmp = *((volatile int *)ptr);
    *((volatile int *)ptr) += val;
    return tmp;
}

void __sync_synchronize()
{
    // gcc doesn't like providing an implementation of this function for arm926ej-s
    // it could be that processor doesn't support out-of-order execution, but it does
    // support the IMB instruction, which may be enough to guarentee non-out-of-order
    // execution.
    __asm__ volatile ("mcr p15, 0, %0, c7, c5, 4"
                          :: "r"(0)
                          : "memory");
}

void vApplicationGetTimerTaskMemory(
    StaticTask_t          **ppxTimerTaskTCBBuffer,
    StackType_t           **ppxTimerTaskStackBuffer,
    configSTACK_DEPTH_TYPE *puxTimerTaskStackSize)
{
    static StaticTask_t tcb;
    *ppxTimerTaskTCBBuffer = &tcb;

    static StackType_t stack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskStackBuffer = stack;
    *puxTimerTaskStackSize
        = (configSTACK_DEPTH_TYPE)configTIMER_TASK_STACK_DEPTH;
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t  **ppxIdleTaskStackBuffer,
                                   configSTACK_DEPTH_TYPE *puxIdleTaskStackSize)
{
    static StaticTask_t tcb;
    *ppxIdleTaskTCBBuffer = &tcb;

    static StackType_t stack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskStackBuffer = stack;
    *puxIdleTaskStackSize   = (configSTACK_DEPTH_TYPE)configMINIMAL_STACK_SIZE;
}

int main(void)
{
    osal_setup();
    xPortStartScheduler();

    while(1)
    {
        // Scheduler running
    }
    
    return 0;
}