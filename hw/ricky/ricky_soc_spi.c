
#include "qemu/osdep.h"
#include "qemu/log.h"
#include "ricky_soc_spi.h"

#define RICKY_SOC_SPI_CR1     0x00
#define RICKY_SOC_SPI_CR2     0x04
#define RICKY_SOC_SPI_SR      0x08
#define RICKY_SOC_SPI_DR      0x0C
#define RICKY_SOC_SPI_CRCPR   0x10
#define RICKY_SOC_SPI_RXCRCR  0x14
#define RICKY_SOC_SPI_TXCRCR  0x18
#define RICKY_SOC_SPI_I2SCFGR 0x1C
#define RICKY_SOC_SPI_I2SPR   0x20

#define RICKY_SOC_SPI_SR_RXNE   1

static void ricky_soc_spi_reset(DeviceState *dev)
{
    RickySocSpiState *s = RICKY_SOC_SPI(dev);

    s->spi_cr1 = 0x00000000;
    s->spi_cr2 = 0x00000000;
    s->spi_sr = 0x00000002;
    s->spi_dr = 0x00000000;
    s->spi_crcpr = 0x00000007;
    s->spi_rxcrcr = 0x00000000;
    s->spi_txcrcr = 0x00000000;
    s->spi_i2scfgr = 0x00000000;
    s->spi_i2spr = 0x00000002;
}

static void ricky_soc_spi_transfer(RickySocSpiState *s)
{
    s->spi_dr = ssi_transfer(s->ssi, s->spi_dr);
    s->spi_sr |= RICKY_SOC_SPI_SR_RXNE;
}

static uint64_t ricky_soc_spi_read(void *opaque, hwaddr addr,
                                   unsigned int size)
{
    RickySocSpiState *s = opaque;
    switch (addr) {
        case RICKY_SOC_SPI_CR1:
            return s->spi_cr1;
        case RICKY_SOC_SPI_CR2:
            qemu_log_mask(LOG_UNIMP, "%s: Interrupts and DMA are not implemented\n",
                          __func__);
            return s->spi_cr2;
        case RICKY_SOC_SPI_SR:
            return s->spi_sr;
        case RICKY_SOC_SPI_DR:
            ricky_soc_spi_transfer(s);
            s->spi_sr &= ~RICKY_SOC_SPI_SR_RXNE;
            return s->spi_dr;
        case RICKY_SOC_SPI_CRCPR:
            qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
            return s->spi_crcpr;
        case RICKY_SOC_SPI_RXCRCR:
            qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
            return s->spi_rxcrcr;
        case RICKY_SOC_SPI_TXCRCR:
            qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
            return s->spi_txcrcr;
        case RICKY_SOC_SPI_I2SCFGR:
            qemu_log_mask(LOG_UNIMP, "%s: I2S is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
            return s->spi_i2scfgr;
        case RICKY_SOC_SPI_I2SPR:
            qemu_log_mask(LOG_UNIMP, "%s: I2S is not implemented, the registers " \
                      "are included for compatibility\n", __func__);
            return s->spi_i2spr;
        default:
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Bad offset 0x%" HWADDR_PRIx "\n",
                          __func__, addr);
    }
    return 0;
}

static void ricky_soc_spi_write(void *opaque, hwaddr addr,
                                uint64_t val64, unsigned int size)
{
    RickySocSpiState *s = opaque;
    uint32_t value = val64;

    switch (addr) {
        case RICKY_SOC_SPI_CR1:
            s->spi_cr1 = value;
            return;
        case RICKY_SOC_SPI_CR2:
            qemu_log_mask(LOG_UNIMP, "%s: " \
                      "Interrupts and DMA are not implemented\n", __func__);
            s->spi_cr2 = value;
            return;
        case RICKY_SOC_SPI_SR:
            /* Read only register, except for clearing the CRCERR bit, which
             * is not supported
             */
            return;
        case RICKY_SOC_SPI_DR:
            s->spi_dr = value;
            ricky_soc_spi_transfer(s);
            return;
        case RICKY_SOC_SPI_CRCPR:
            qemu_log_mask(LOG_UNIMP, "%s: CRC is not implemented\n", __func__);
            return;
        case RICKY_SOC_SPI_RXCRCR:
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: " \
                      "0x%" HWADDR_PRIx "\n", __func__, addr);
            return;
        case RICKY_SOC_SPI_TXCRCR:
            qemu_log_mask(LOG_GUEST_ERROR, "%s: Read only register: " \
                      "0x%" HWADDR_PRIx "\n", __func__, addr);
            return;
        case RICKY_SOC_SPI_I2SCFGR:
            qemu_log_mask(LOG_UNIMP, "%s: " \
                      "I2S is not implemented\n", __func__);
            return;
        case RICKY_SOC_SPI_I2SPR:
            qemu_log_mask(LOG_UNIMP, "%s: " \
                      "I2S is not implemented\n", __func__);
            return;
        default:
            qemu_log_mask(LOG_GUEST_ERROR,
                          "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps ricky_soc_spi_ops = {
        .read = ricky_soc_spi_read,
        .write = ricky_soc_spi_write,
        .endianness = DEVICE_NATIVE_ENDIAN,
};



static void ricky_soc_spi_init(Object *obj)
{
    RickySocSpiState *s = RICKY_SOC_SPI(obj);
    DeviceState *dev = DEVICE(obj);

    memory_region_init_io(&s->mmio, obj, &ricky_soc_spi_ops, s,
                          TYPE_RICKY_SOC_SPI, 0x400);

    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);
    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq_cs);

    s->ssi = ssi_create_bus(dev, "ssi");
}

static void ricky_soc_spi_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    device_class_set_legacy_reset(dc, ricky_soc_spi_reset);
}


static const TypeInfo ricky_soc_spi_info = {
        .name          = TYPE_RICKY_SOC_SPI,
        .parent        = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(RickySocSpiState ),
        .instance_init = ricky_soc_spi_init,
        .class_init    = ricky_soc_spi_class_init,
};

static void ricky_soc_spi_register_types(void)
{
    type_register_static(&ricky_soc_spi_info);
}

type_init(ricky_soc_spi_register_types)


void ricky_soc_spi_initialize_child(Object *obj, RickySocSpiState *spi)
{
    object_initialize_child(obj, "spi[*]", spi, TYPE_RICKY_SOC_SPI);
}


void ricky_soc_spi_create(RickySocSpiState *spi, qemu_irq irq, hwaddr base, Error **errp)
{
    DeviceState *dev;
    SysBusDevice *busdev;

    dev = DEVICE(spi);
    if (!sysbus_realize(SYS_BUS_DEVICE(spi), errp)) {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, base);
    sysbus_connect_irq(busdev, 0, irq);
}

