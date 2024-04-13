static void mptsas_scsi_uninit(PCIDevice *dev)
{
    MPTSASState *s = MPT_SAS(dev);

    qemu_bh_delete(s->request_bh);
    msi_uninit(dev);
}