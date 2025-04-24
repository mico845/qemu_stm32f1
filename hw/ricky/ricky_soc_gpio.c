#include "qemu/osdep.h"
#include "ricky_soc_gpio.h"



static void ricky_soc_gpio_init(Object *obj)
{

}

static void ricky_soc_gpio_class_init(ObjectClass *klass, void *data)
{

}

static const TypeInfo ricky_soc_gpio_info = {
        .name          = TYPE_RICKY_SOC_GPIO,
        .parent        = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(RickySocGpioState),
        .instance_init = ricky_soc_gpio_init,
        .class_init    = ricky_soc_gpio_class_init,
};

static void ricky_soc_gpio_register_types(void)
{
    type_register_static(&ricky_soc_gpio_info);
}

type_init(ricky_soc_gpio_register_types)

