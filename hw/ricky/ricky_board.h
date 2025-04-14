#ifndef QEMU_RICKY_BOARD_H
#define QEMU_RICKY_BOARD_H

#include "hw/boards.h"
#include "hw/sysbus.h"
#include "qom/object.h"

#define TYPE_RICKY_BOARD MACHINE_TYPE_NAME("ricky")
typedef struct RickyBoardState RickyBoardState;
#define RISCV_RICKY_STAR_MACHINE(obj) \
OBJECT_CHECK(RickyStarState, (obj), TYPE_RICKY_BOARD)

struct RickyBoardState {
    /*< private >*/
    MachineState parent;

};

#endif //QEMU_RICKY_BOARD_H
