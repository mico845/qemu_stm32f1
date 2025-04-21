#ifndef QEMU_RICKY_BOARD_H
#define QEMU_RICKY_BOARD_H

#include "ricky_soc.h"

#define TYPE_RICKY_BOARD MACHINE_TYPE_NAME("RickyBoard")
typedef struct RickyBoardState RickyBoardState;
#define RICKY_BOARD(obj) \
OBJECT_CHECK(RickyBoardState, (obj), TYPE_RICKY_BOARD)

#define FLASH_SIZE (128 * 1024)

struct RickyBoardState {
    /*< private >*/
    MachineState parent;

    /*< public >*/
    RickySocState soc;
};

#endif //QEMU_RICKY_BOARD_H
