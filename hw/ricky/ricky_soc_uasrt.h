#ifndef QEMU_RICKY_SOC_UASRT_H
#define QEMU_RICKY_SOC_UASRT_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

typedef struct RickySocUsartState RickySocUsartState;
#define TYPE_RICKY_SOC_USART "RickyBoard-usart"
#define RICKY_SOC_USART(obj) \
OBJECT_CHECK(RickySocUsartState, (obj), TYPE_RICKY_SOC_USART)

struct RickySocUsartState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t usart_sr;
    uint32_t usart_dr;
    uint32_t usart_brr;
    uint32_t usart_cr1;
    uint32_t usart_cr2;
    uint32_t usart_cr3;
    uint32_t usart_gtpr;

    CharBackend chr;
    qemu_irq irq;
};

void ricky_soc_usart_initialize_child(Object *obj, RickySocUsartState *usart);
void ricky_soc_usart_create(RickySocUsartState *usart, int serial_id, qemu_irq irq, hwaddr base, Error **errp);

#endif //QEMU_RICKY_SOC_UASRT_H
