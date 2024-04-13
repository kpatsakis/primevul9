static void rtl8139_instance_init(Object *obj)
{
    RTL8139State *s = RTL8139(obj);

    device_add_bootindex_property(obj, &s->conf.bootindex,
                                  "bootindex", "/ethernet-phy@0",
                                  DEVICE(obj));
}