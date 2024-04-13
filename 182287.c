static void vmxnet3_instance_init(Object *obj)
{
    VMXNET3State *s = VMXNET3(obj);
    device_add_bootindex_property(obj, &s->conf.bootindex,
                                  "bootindex", "/ethernet-phy@0",
                                  DEVICE(obj));
}