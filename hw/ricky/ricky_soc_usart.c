#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qapi/error.h"
#include "system/system.h"

#include "ricky_soc_uasrt.h"

#define USART_SR   0x00
#define USART_DR   0x04
#define USART_BRR  0x08
#define USART_CR1  0x0C
#define USART_CR2  0x10
#define USART_CR3  0x14
#define USART_GTPR 0x18

#define USART_SR_TXE  (1 << 7)
#define USART_SR_TC   (1 << 6)
#define USART_SR_RXNE (1 << 5)

#define USART_SR_RESET (USART_SR_TXE | USART_SR_TC)

static int ricky_soc_usart_can_receive(void *opaque)
{
    RickySocUsartState *s = RICKY_SOC_USART(opaque);

    if (!(s->usart_sr & USART_SR_RXNE)) {
        return 1;
    }

    return 0;
}

static void ricky_soc_usart_receive(void *opaque, const uint8_t *buf, int size)
{
    //   RickySocUsartState *s = RICKY_SOC_USART(opaque);
    //    DeviceState *d = DEVICE(s);
}


static void ricky_soc_usart_update_irq(RickySocUsartState *s)
{
    uint32_t mask = s->usart_sr & s->usart_cr1;

    if (mask & (USART_SR_TXE | USART_SR_TC | USART_SR_RXNE)) {
        qemu_set_irq(s->irq, 1);
    } else {
        qemu_set_irq(s->irq, 0);
    }
}

static void ricky_soc_usart_reset(DeviceState *dev)
{
    RickySocUsartState *s = RICKY_SOC_USART(dev);

    s->usart_sr = USART_SR_RESET;
    s->usart_dr = 0x00000000;
    s->usart_brr = 0x00000000;
    s->usart_cr1 = 0x00000000;
    s->usart_cr2 = 0x00000000;
    s->usart_cr3 = 0x00000000;
    s->usart_gtpr = 0x00000000;

    ricky_soc_usart_update_irq(s);
}

static void ricky_soc_usart_write(void *opaque, hwaddr addr,
                                  uint64_t val64, unsigned int size)
{
    RickySocUsartState *s = RICKY_SOC_USART(opaque);
    uint32_t value = val64;
    unsigned char ch;

    switch (addr) {
        case USART_SR:
            if (value <= 0x3FF) {
                /* I/O being synchronous, TXE is always set. In addition, it may
                   only be set by hardware, so keep it set here. */
                s->usart_sr = value | USART_SR_TXE;
            } else {
                s->usart_sr &= value;
            }
            ricky_soc_usart_update_irq(s);
            return;
        case USART_DR:
            if (value < 0xF000) {
                ch = value;
                /* XXX this blocks entire thread. Rewrite to use
                 * qemu_chr_fe_write and background I/O callbacks */
                qemu_chr_fe_write_all(&s->chr, &ch, 1);
                /* XXX I/O are currently synchronous, making it impossible for
                   software to observe transient states where TXE or TC aren't
                   set. Unlike TXE however, which is read-only, software may
                   clear TC by writing 0 to the SR register, so set it again
                   on each write. */
                s->usart_sr |= USART_SR_TC;
                ricky_soc_usart_update_irq(s);
            }
            return;
        case USART_BRR:
            s->usart_brr = value;
            return;
        case USART_CR1:
            s->usart_cr1 = value;
            ricky_soc_usart_update_irq(s);
            return;
        case USART_CR2:
            s->usart_cr2 = value;
            return;
        case USART_CR3:
            s->usart_cr3 = value;
            return;
        case USART_GTPR:
            s->usart_gtpr = value;
            return;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, addr);
    }
}

static uint64_t ricky_soc_usart_read(void *opaque, hwaddr addr,
                                     unsigned int size)
{
    RickySocUsartState *s = RICKY_SOC_USART(opaque);

    uint64_t retvalue = 0;
    switch (addr) {
        case USART_SR:
            retvalue = s->usart_sr;
            qemu_chr_fe_accept_input(&s->chr);
            break;
        case USART_DR:
            retvalue = s->usart_dr & 0x3FF;
            s->usart_sr &= ~USART_SR_RXNE;
            qemu_chr_fe_accept_input(&s->chr);
            ricky_soc_usart_update_irq(s);
            break;
        case USART_BRR:
            retvalue = s->usart_brr;
            break;
        case USART_CR1:
            retvalue = s->usart_cr1;
            break;
        case USART_CR2:
            retvalue = s->usart_cr2;
            break;
        case USART_CR3:
            retvalue = s->usart_cr3;
            break;
        case USART_GTPR:
            retvalue = s->usart_gtpr;
            break;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, addr);
            return 0;
    }

    return retvalue;
}

static const MemoryRegionOps ricky_soc_usart_ops = {
        .read = ricky_soc_usart_read,
        .write = ricky_soc_usart_write,
        .endianness = DEVICE_NATIVE_ENDIAN,
};

static const Property ricky_soc_usart_properties[] = {
        DEFINE_PROP_CHR("chardev", RickySocUsartState, chr),
};

static void ricky_soc_usart_init(Object *obj)
{
    RickySocUsartState *s = RICKY_SOC_USART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &ricky_soc_usart_ops, s,
                          TYPE_RICKY_SOC_USART, 0x400);

    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void ricky_soc_usart_realize(DeviceState *dev, Error **errp)
{

    RickySocUsartState *s = RICKY_SOC_USART(dev);

    qemu_chr_fe_set_handlers(&s->chr, ricky_soc_usart_can_receive,
                             ricky_soc_usart_receive, NULL, NULL,
                             s, NULL, true);
}

static void ricky_soc_usart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_legacy_reset(dc, ricky_soc_usart_reset);
    device_class_set_props(dc, ricky_soc_usart_properties);
    dc->realize = ricky_soc_usart_realize;
}

static const TypeInfo ricky_soc_usart_info = {
        .name          = TYPE_RICKY_SOC_USART,
        .parent        = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(RickySocUsartState),
        .instance_init = ricky_soc_usart_init,
        .class_init    = ricky_soc_usart_class_init,
};

static void ricky_soc_usart_register_types(void)
{
    type_register_static(&ricky_soc_usart_info);
}

type_init(ricky_soc_usart_register_types)


void ricky_soc_usart_initialize_child(Object *obj, RickySocUsartState *usart)
{
    object_initialize_child(obj, "usart[*]", usart,
                            TYPE_RICKY_SOC_USART);
}


void ricky_soc_usart_create(RickySocUsartState *usart, int serial_id, qemu_irq irq, hwaddr base, Error **errp)
{
    DeviceState *dev;
    SysBusDevice *busdev;
    char filename[64];
    snprintf(filename, sizeof(filename), "file:output/uart%d.log", serial_id);
    char chardevname[64];
    snprintf(chardevname, sizeof(chardevname), "ricky_usart_chr%d", serial_id);
    Chardev *chardev = qemu_chr_new(chardevname, filename, NULL);

    dev = DEVICE(usart);
    // qdev_prop_set_chr(dev, "chardev", serial_hd(serial_id));
    qdev_prop_set_chr(dev, "chardev", chardev);
    if (!sysbus_realize(SYS_BUS_DEVICE(usart), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, base);
    sysbus_connect_irq(busdev, 0, irq);
}
