#ifndef QEMU_RICKY_SOC_SPI_H
#define QEMU_RICKY_SOC_SPI_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

typedef struct RickySocSpiState RickySocSpiState;
#define TYPE_RICKY_SOC_SPI "RickyBoard-usart"
#define RICKY_SOC_SPI(obj) \
OBJECT_CHECK(RickySocSpiState, (obj), TYPE_RICKY_SOC_SPI)

struct RickySocSpiState {
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t spi_cr1;
    uint32_t spi_cr2;
    uint32_t spi_sr;
    uint32_t spi_dr;
    uint32_t spi_crcpr;
    uint32_t spi_rxcrcr;
    uint32_t spi_txcrcr;
    uint32_t spi_i2scfgr;
    uint32_t spi_i2spr;

};


#endif //QEMU_RICKY_SOC_SPI_H
