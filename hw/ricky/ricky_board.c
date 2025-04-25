#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "qom/object.h"
#include "hw/qdev-clock.h"
#include "qemu/log-for-trace.h"
#include "hw/arm/boot.h"
#include "ricky_board.h"


/* Main SYSCLK frequency in Hz (24MHz) */
#define SYSCLK_FRQ 24000000ULL

static void ricky_board_init(MachineState *machine)
{
    Clock *sysclk;
    RickyBoardState *board = RICKY_BOARD(machine);
    sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(sysclk, SYSCLK_FRQ);
    object_initialize_child(OBJECT(machine), "ricky_board.soc", &board->soc,
                            TYPE_RICKY_SOC);
    qdev_connect_clock_in(DEVICE(&board->soc), "sysclk", sysclk);

    sysbus_realize_and_unref(SYS_BUS_DEVICE(&board->soc), &error_fatal);

    board_spi_flash_m25p80_create(OBJECT(&board->soc.spi));

    armv7m_load_kernel(board->soc.armv7m.cpu,
                       machine->kernel_filename,
                       0x00, FLASH_SIZE);
}

static void ricky_board_class_init(ObjectClass *oc, void *data)
{
    static const char * const valid_cpu_types[] = {
            ARM_CPU_TYPE_NAME("cortex-m3"),
            NULL
    };
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->init = ricky_board_init;
    mc->desc = "Ricky Star Board";
    mc->valid_cpu_types = valid_cpu_types;
}

static const TypeInfo ricky_board_typeinfo = {
    .name       = TYPE_RICKY_BOARD,
    .parent     = TYPE_MACHINE,
    .class_init = ricky_board_class_init,
    .instance_size = sizeof(RickyBoardState),
};

static void ricky_board_init_register_types(void)
{
    type_register_static(&ricky_board_typeinfo);
}

type_init(ricky_board_init_register_types)



DeviceState *board_spi_flash_m25p80_create(Object *obj)
{
    RickySocSpiState *s = RICKY_SOC_SPI(obj);
    DeviceState *dev = DEVICE(obj);
    DeviceState *sdev;
    SSIBus *spi = (SSIBus *)qdev_get_child_bus(dev, "ssi");

    sdev = qdev_new("w25q64");
    // sdev = qdev_new("n25q256a");

    qdev_realize_and_unref(sdev, BUS(spi), &error_fatal);

    s->irq_cs = qdev_get_gpio_in_named(sdev, SSI_GPIO_CS, 0);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 1, s->irq_cs);

    return sdev;
}