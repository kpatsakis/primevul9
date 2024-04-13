static void mptsas1068_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    PCIDeviceClass *pc = PCI_DEVICE_CLASS(oc);

    pc->realize = mptsas_scsi_realize;
    pc->exit = mptsas_scsi_uninit;
    pc->romfile = 0;
    pc->vendor_id = PCI_VENDOR_ID_LSI_LOGIC;
    pc->device_id = PCI_DEVICE_ID_LSI_SAS1068;
    pc->subsystem_vendor_id = PCI_VENDOR_ID_LSI_LOGIC;
    pc->subsystem_id = 0x8000;
    pc->class_id = PCI_CLASS_STORAGE_SCSI;
    device_class_set_props(dc, mptsas_properties);
    dc->reset = mptsas_reset;
    dc->vmsd = &vmstate_mptsas;
    dc->desc = "LSI SAS 1068";
    set_bit(DEVICE_CATEGORY_STORAGE, dc->categories);
}