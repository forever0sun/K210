/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtthread.h>

#include <port.h>
#include "board.h"
#include "drv_uart.h"

#include "encoding.h"
#include "fpioa.h"
#include "sysclock.h"
#include "dmac.h"

#include "uarths.h"
void rt_hw_console_output(const char *str)
{
    uart_puts(str);
    return ;
}

void init_bss(void)
{
    unsigned int *dst;

    dst = &__bss_start;
    while (dst < &__bss_end)
    {
        *dst++ = 0;
    }
}

void cpu_entry(int cpuid)
{
    extern void entry(void);

    if (cpuid == 0)
    {
        init_bss();
        entry();
    }
    else
    {
        while (1) ;
    }
}

#include <clint.h>
#include <sysctl.h>

int freq(void)
{
    rt_uint64_t value = clint_timer_get_freq();
    rt_kprintf("TIMER: %d\n", value);

    value = sysctl_get_freq();
    rt_kprintf("SYS : %d\n", value);

    value = sysctl_clock_get_freq(SYSCTL_CLOCK_PLL0);
    rt_kprintf("PLL0: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_PLL1);
    rt_kprintf("PLL1: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_PLL2);
    rt_kprintf("PLL2: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
    rt_kprintf("CPU : %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_APB0);
    rt_kprintf("APB0: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_APB1);
    rt_kprintf("APB1: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_APB2);
    rt_kprintf("APB2: %d\n", value);
    value = sysctl_clock_get_freq(SYSCTL_CLOCK_HCLK);
    rt_kprintf("HCLK: %d\n", value);

    value = clint_get_time();
    rt_kprintf("mtime: %d\n", value);
    value = clint_get_cmp();
    rt_kprintf("mcmp : %d\n", value);

    return 0;
}
MSH_CMD_EXPORT(freq, show freq info);

int list_csr(void)
{
    uint32_t value = read_csr(mcause);
    rt_kprintf("mcause: 0x%08x\n");
    return 0;
}
MSH_CMD_EXPORT(list_csr, show CSR reg);

void rt_hw_board_init(void)
{
    /* Init FPIOA */
    fpioa_init();
    /* PLL init */
    sys_clock_init();
    /* Dmac init */
    dmac_init();

    /* initalize interrupt */
    rt_hw_interrupt_init();
    /* initialize hardware interrupt */
    rt_hw_uart_init();
    // rt_hw_tick_init();

#ifdef RT_USING_CONSOLE
    /* set console device */
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif /* RT_USING_CONSOLE */

#ifdef RT_USING_HEAP
    rt_kprintf("heap: [0x%08x - 0x%08x]\n", (rt_ubase_t) RT_HW_HEAP_BEGIN, (rt_ubase_t) RT_HW_HEAP_END);
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}