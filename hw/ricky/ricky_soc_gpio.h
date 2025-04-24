#ifndef QEMU_RICKY_SOC_GPIO_H
#define QEMU_RICKY_SOC_GPIO_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

typedef struct RickySocGpioState RickySocGpioState;
#define TYPE_RICKY_SOC_GPIO "RickyBoard-gpio"
#define RICKY_SOC_GPIO(obj) \
OBJECT_CHECK(RickySocGpioState, (obj), TYPE_RICKY_SOC_GPIO)

struct RickySocGpioState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

};


#endif //QEMU_RICKY_SOC_GPIO_H
