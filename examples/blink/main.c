#include "stm32f4xx_hal.h"

int main(void)
{
    RCC_OscInitTypeDef OscInitType = {};
    RCC_ClkInitTypeDef ClkInitType = {};
    GPIO_InitTypeDef GPIOInitType = {};

    HAL_Init();

    OscInitType.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    OscInitType.HSEState = RCC_HSE_ON;
    OscInitType.PLL.PLLState = RCC_PLL_ON;
    OscInitType.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    OscInitType.PLL.PLLM = 4;
    OscInitType.PLL.PLLN = 84;
    OscInitType.PLL.PLLP = 2;
    HAL_RCC_OscConfig(&OscInitType);

    ClkInitType.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                            RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    ClkInitType.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    ClkInitType.AHBCLKDivider = RCC_SYSCLK_DIV1;
    ClkInitType.APB1CLKDivider = RCC_HCLK_DIV2;
    ClkInitType.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&ClkInitType, FLASH_LATENCY_1);

    GPIOInitType.Pin = GPIO_PIN_13;
    GPIOInitType.Mode = GPIO_MODE_OUTPUT_PP;
    GPIOInitType.Speed = GPIO_SPEED_FREQ_HIGH;

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_Init(GPIOC, &GPIOInitType);

    for (;;) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(500);
    }

    return 0;
}
