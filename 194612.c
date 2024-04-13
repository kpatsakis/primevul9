static void qxl_realize_secondary(PCIDevice *dev, Error **errp)
{
    PCIQXLDevice *qxl = PCI_QXL(dev);

    qxl_init_ramsize(qxl);
    memory_region_init_ram(&qxl->vga.vram, OBJECT(dev), "qxl.vgavram",
                           qxl->vga.vram_size, &error_fatal);
    qxl->vga.vram_ptr = memory_region_get_ram_ptr(&qxl->vga.vram);
    qxl->vga.con = graphic_console_init(DEVICE(dev), 0, &qxl_ops, qxl);
    qxl->id = qemu_console_get_index(qxl->vga.con); /* == channel_id */

    qxl_realize_common(qxl, errp);
}