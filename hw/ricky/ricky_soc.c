#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "qemu/log-for-trace.h"
#include "exec/address-spaces.h"
#include "hw/misc/unimp.h"
#include "hw/qdev-clock.h"

#include "ricky_board.h"

static const MemMapEntry base_memmap[] = {
        [RICKY_MEM_BOOT]            =   { 0x00000000,   0x8000000 },
        [RICKY_MEM_FLASH]           =   { 0x08000000,  FLASH_SIZE },
        [RICKY_MEM_SYSTEM_MEM]      =   { 0x1FFFF000,       0x800 },
        [RICKY_MEM_OPTION_BYTES]    =   { 0x1FFFF800,         0xF },
        [RICKY_MEM_SRAM]            =   { 0x20000000,     0x20000 }, // 8 *1024


        [RICKY_MEM_TIM2]            =   { 0x40000000,       0x400 },
        [RICKY_MEM_TIM3]            =   { 0x40000400,       0x400 },
        [RICKY_MEM_TIM4]            =   { 0x40000800,       0x400 },
        [RICKY_MEM_RTC]             =   { 0x40002800,       0x400 },
        [RICKY_MEM_WWDG]            =   { 0x40002C00,       0x400 },
        [RICKY_MEM_IWDG]            =   { 0x40003000,       0x400 },
        [RICKY_MEM_SPI2]            =   { 0x40003800,       0x400 },
        [RICKY_MEM_USART2]          =   { 0x40004400,       0x400 },
        [RICKY_MEM_USART3]          =   { 0x40004800,       0x400 },
        [RICKY_MEM_I2C1]            =   { 0x40005400,       0x400 },
        [RICKY_MEM_I2C2]            =   { 0x40005800,       0x400 },
        [RICKY_MEM_USB]             =   { 0x40005C00,       0x400 },
        [RICKY_MEM_USB_CAN_SRAM]    =   { 0x40006000,       0x400 },
        [RICKY_MEM_BXCAN]           =   { 0x40006400,       0x400 },
        [RICKY_MEM_BKP]             =   { 0x40006C00,       0x400 },
        [RICKY_MEM_PWR]             =   { 0x40007000,       0x400 },

        [RICKY_MEM_AFIO]            =   { 0x40010000,       0x400 },
        [RICKY_MEM_EXTI]            =   { 0x40010400,       0x400 },
        [RICKY_MEM_GPIOA]           =   { 0x40010800,       0x400 },
        [RICKY_MEM_GPIOB]           =   { 0x40010C00,       0x400 },
        [RICKY_MEM_GPIOC]           =   { 0x40011000,       0x400 },
        [RICKY_MEM_GPIOD]           =   { 0x40011400,       0x400 },
        [RICKY_MEM_GPIOE]           =   { 0x40011800,       0x400 },
        [RICKY_MEM_ADC1]            =   { 0x40012400,       0x400 },
        [RICKY_MEM_ADC2]            =   { 0x40012800,       0x400 },

        [RICKY_MEM_TIM1]            =   { 0x40012C00,       0x400 },
        [RICKY_MEM_SPI1]            =   { 0x40013000,       0x400 },
        [RICKY_MEM_USART1]          =   { 0x40013800,       0x400 },

        [RICKY_MEM_DMA]             =   { 0x40020000,       0x400 },
        [RICKY_MEM_RCC]             =   { 0x40021000,       0x400 },
        [RICKY_MEM_FLASH_INT]       =   { 0x40022000,       0x400 },
        [RICKY_MEM_CRC]             =   { 0x40023000,       0x400 },
};


static void ricky_soc_realize(DeviceState *dev_soc, Error **errp)
{
    qemu_log("ricky soc_realize\r\n");
    DeviceState *armv7m;
    RickySocState *s = RICKY_SOC(dev_soc);
    MemoryRegion *system_memory = get_system_memory();

    if (clock_has_source(s->refclk)) {
        error_setg(errp, "refclk clock must not be wired up by the board code");
        return;
    }

    if (!clock_has_source(s->sysclk)) {
        error_setg(errp, "sysclk clock must be wired up by the board code");
        return;
    }

    /*
     * TODO: ideally we should model the SoC RCC and its ability to
     * change the sysclk frequency and define different sysclk sources.
     */

    /* The refclk always runs at frequency HCLK / 8 */
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);


    memory_region_init_rom(&s->flash, OBJECT(dev_soc), "ricky-soc.flash",
                           base_memmap[RICKY_MEM_FLASH].size, &error_fatal);
    memory_region_add_subregion(system_memory, base_memmap[RICKY_MEM_FLASH].base, &s->flash);


    memory_region_init_ram(&s->sram, NULL, "ricky-soc.sram", base_memmap[RICKY_MEM_SRAM].size,
                           &error_fatal);
    memory_region_add_subregion(system_memory, base_memmap[RICKY_MEM_SRAM].base, &s->sram);

    // Aliased to flash or system memory depending on BOOT pins
    memory_region_init_alias(&s->boot_alias, OBJECT(dev_soc),
                             "ricky-soc.boot", &s->flash, base_memmap[RICKY_MEM_BOOT].base, base_memmap[RICKY_MEM_FLASH].size);
    memory_region_add_subregion(system_memory, base_memmap[RICKY_MEM_BOOT].base, &s->boot_alias);

    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq", 61);
    qdev_prop_set_uint8(armv7m, "num-prio-bits", 4);
    qdev_prop_set_string(armv7m, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m3"));
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->refclk);
    object_property_set_link(OBJECT(&s->armv7m), "memory",
                             OBJECT(system_memory), &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }

    create_unimplemented_device("timer[2]",  base_memmap[RICKY_MEM_TIM2].base, base_memmap[RICKY_MEM_TIM2].size);

}

static void ricky_soc_initfn(Object *obj)
{
    qemu_log("ricky soc_instance_init\r\n");

    RickySocState *s = RICKY_SOC(obj);
    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
    s->refclk = qdev_init_clock_in(DEVICE(s), "refclk", NULL, NULL, 0);
}


static void ricky_soc_class_init(ObjectClass *klass, void *data)
{
    qemu_log("ricky soc_class_init\r\n");
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->realize = ricky_soc_realize;
}

static const TypeInfo ricky_soc_info = {
        .name          = TYPE_RICKY_SOC,
        .parent        = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(RickySocState),
        .instance_init = ricky_soc_initfn,
        .class_init    = ricky_soc_class_init,
};

static void ricky_soc_types(void)
{
    type_register_static(&ricky_soc_info);
}

type_init(ricky_soc_types)