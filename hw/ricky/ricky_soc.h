#ifndef QEMU_RICKY_SOC_H
#define QEMU_RICKY_SOC_H

#include "hw/boards.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/arm/armv7m.h"

#include "ricky_soc_uasrt.h"

#define TYPE_RICKY_SOC "RickyBoard-soc"
typedef struct RickySocState RickySocState;
#define RICKY_SOC(obj) \
OBJECT_CHECK(RickySocState, (obj), TYPE_RICKY_SOC)

#define RICKY_SOC_USART_NUM 1

struct RickySocState {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    ARMv7MState armv7m;

    MemoryRegion sram;
    MemoryRegion flash;
    MemoryRegion boot_alias;

    RickySocUsartState usart[RICKY_SOC_USART_NUM];

    Clock *sysclk;
    Clock *refclk;
};

enum {
    RICKY_MEM_BOOT,
    RICKY_MEM_FLASH,
    RICKY_MEM_SYSTEM_MEM,
    RICKY_MEM_OPTION_BYTES,
    RICKY_MEM_SRAM,

    RICKY_MEM_TIM2,
    RICKY_MEM_TIM3,
    RICKY_MEM_TIM4,
    RICKY_MEM_TIM6,
    RICKY_MEM_TIM7,
    RICKY_MEM_RTC,
    RICKY_MEM_WWDG,
    RICKY_MEM_IWDG,
    RICKY_MEM_SPI2,
    RICKY_MEM_USART2,
    RICKY_MEM_USART3,
    RICKY_MEM_I2C1,
    RICKY_MEM_I2C2,
    RICKY_MEM_USB,
    RICKY_MEM_USB_CAN_SRAM,
    RICKY_MEM_BXCAN,
    RICKY_MEM_BKP,
    RICKY_MEM_PWR,
    RICKY_MEM_DAC,
    RICKY_MEM_CEC,

    RICKY_MEM_AFIO,
    RICKY_MEM_EXTI,
    RICKY_MEM_GPIOA,
    RICKY_MEM_GPIOB,
    RICKY_MEM_GPIOC,
    RICKY_MEM_GPIOD,
    RICKY_MEM_GPIOE,
    RICKY_MEM_ADC1,
    RICKY_MEM_ADC2,
    RICKY_MEM_TIM1,
    RICKY_MEM_SPI1,
    RICKY_MEM_USART1,

    RICKY_MEM_DMA,
    RICKY_MEM_RCC,
    RICKY_MEM_FLASH_INT,
    RICKY_MEM_CRC,

    RICKY_MEM_NUM
};


#endif //QEMU_RICKY_SOC_H
