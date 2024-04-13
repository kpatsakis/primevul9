static bool vmxnet3_verify_driver_magic(PCIDevice *d, hwaddr dshmem)
{
    return (VMXNET3_READ_DRV_SHARED32(d, dshmem, magic) == VMXNET3_REV1_MAGIC);
}