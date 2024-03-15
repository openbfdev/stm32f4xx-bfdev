/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021-2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "benchmark"
#define bfdev_log_fmt(fmt) MODULE_NAME ": " fmt

#include <stdio.h>
#include <errno.h>
#include <bfdev.h>

#include "main.h"
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart1;
IWDG_HandleTypeDef hiwgd;

extern int crc_benchmark(void);
extern int rbtree_benchmark(void);
extern int mpi_benchmark(void);

int __io_putchar(int ch)
{
    if (ch == '\n')
        HAL_UART_Transmit(&huart1, (void *)"\r", 1, -1);
    HAL_UART_Transmit(&huart1, (void *)&ch, 1, -1);
    return ch;
}

void _exit(int status)
{
    printf("Benchmark panic: (%d)\n", errno);
    for (;;)
        iwdg_touch();
}

int main(void)
{
    RCC_OscInitTypeDef OscInitType = {};
    RCC_ClkInitTypeDef ClkInitType = {};
    GPIO_InitTypeDef GPIOInitType = {};
    const char *errinfo;
    int retval;

    HAL_Init();

    OscInitType.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSI;
    OscInitType.HSEState = RCC_HSE_ON;
    OscInitType.LSIState = RCC_LSI_ON;
    OscInitType.PLL.PLLState = RCC_PLL_ON;
    OscInitType.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    OscInitType.PLL.PLLM = 4;
    OscInitType.PLL.PLLN = 84;
    OscInitType.PLL.PLLP = 2;
    HAL_RCC_OscConfig(&OscInitType);
    HAL_RCC_EnableCSS();

    ClkInitType.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    ClkInitType.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    ClkInitType.AHBCLKDivider = RCC_SYSCLK_DIV1;
    ClkInitType.APB1CLKDivider = RCC_HCLK_DIV2;
    ClkInitType.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&ClkInitType, FLASH_LATENCY_2);

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    GPIOInitType.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIOInitType.Mode = GPIO_MODE_AF_PP;
    GPIOInitType.Pull = GPIO_PULLUP;
    GPIOInitType.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIOInitType.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIOInitType);

    GPIOInitType.Pin = GPIO_PIN_13;
    GPIOInitType.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOC, &GPIOInitType);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart1);

    hiwgd.Instance = IWDG;
    hiwgd.Init.Prescaler = IWDG_PRESCALER_32;
    hiwgd.Init.Reload = 1500;
    HAL_IWDG_Init(&hiwgd);

    bfdev_log_info("Benchmark for STM32F4xx.\n");
    bfdev_log_info("Bfdev version: %s\n", __bfdev_stringify(BFDEV_VERSION));
    bfdev_log_info("SYSCLK freq: %u\n", HAL_RCC_GetSysClockFreq());
    bfdev_log_info("AHB freq: %u\n", HAL_RCC_GetHCLKFreq());
    bfdev_log_info("APB1 freq: %u\n", HAL_RCC_GetPCLK1Freq());
    bfdev_log_info("APB2 freq: %u\n", HAL_RCC_GetPCLK2Freq());
    bfdev_log_info("This may take a few minutes...\n");
    puts(""); /* '\n' */

    for (;;) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        iwdg_touch();
        retval = crc_benchmark();
        if (retval) {
            bfdev_errname(retval, &errinfo);
            printf("error %d: %s\n", retval, errinfo);
            abort();
        }
        puts(""); /* '\n' */

        iwdg_touch();
        retval = rbtree_benchmark();
        if (retval) {
            bfdev_errname(retval, &errinfo);
            printf("error %d: %s\n", retval, errinfo);
            abort();
        }
        puts(""); /* '\n' */

        iwdg_touch();
        retval = mpi_benchmark();
        if (retval) {
            bfdev_errname(retval, &errinfo);
            printf("error %d: %s\n", retval, errinfo);
            abort();
        }
        puts(""); /* '\n' */
    }

    return 0;
}
