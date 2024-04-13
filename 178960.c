static void ati_vga_realize(PCIDevice *dev, Error **errp)
{
    ATIVGAState *s = ATI_VGA(dev);
    VGACommonState *vga = &s->vga;

    if (s->model) {
        int i;
        for (i = 0; i < ARRAY_SIZE(ati_model_aliases); i++) {
            if (!strcmp(s->model, ati_model_aliases[i].name)) {
                s->dev_id = ati_model_aliases[i].dev_id;
                break;
            }
        }
        if (i >= ARRAY_SIZE(ati_model_aliases)) {
            warn_report("Unknown ATI VGA model name, "
                        "using default rage128p");
        }
    }
    if (s->dev_id != PCI_DEVICE_ID_ATI_RAGE128_PF &&
        s->dev_id != PCI_DEVICE_ID_ATI_RADEON_QY) {
        error_setg(errp, "Unknown ATI VGA device id, "
                   "only 0x5046 and 0x5159 are supported");
        return;
    }
    pci_set_word(dev->config + PCI_DEVICE_ID, s->dev_id);

    if (s->dev_id == PCI_DEVICE_ID_ATI_RADEON_QY &&
        s->vga.vram_size_mb < 16) {
        warn_report("Too small video memory for device id");
        s->vga.vram_size_mb = 16;
    }

    /* init vga bits */
    vga_common_init(vga, OBJECT(s));
    vga_init(vga, OBJECT(s), pci_address_space(dev),
             pci_address_space_io(dev), true);
    vga->con = graphic_console_init(DEVICE(s), 0, s->vga.hw_ops, &s->vga);
    if (s->cursor_guest_mode) {
        vga->cursor_invalidate = ati_cursor_invalidate;
        vga->cursor_draw_line = ati_cursor_draw_line;
    }

    /* ddc, edid */
    I2CBus *i2cbus = i2c_init_bus(DEVICE(s), "ati-vga.ddc");
    bitbang_i2c_init(&s->bbi2c, i2cbus);
    I2CSlave *i2cddc = I2C_SLAVE(qdev_create(BUS(i2cbus), TYPE_I2CDDC));
    i2c_set_slave_address(i2cddc, 0x50);

    /* mmio register space */
    memory_region_init_io(&s->mm, OBJECT(s), &ati_mm_ops, s,
                          "ati.mmregs", 0x4000);
    /* io space is alias to beginning of mmregs */
    memory_region_init_alias(&s->io, OBJECT(s), "ati.io", &s->mm, 0, 0x100);

    pci_register_bar(dev, 0, PCI_BASE_ADDRESS_MEM_PREFETCH, &vga->vram);
    pci_register_bar(dev, 1, PCI_BASE_ADDRESS_SPACE_IO, &s->io);
    pci_register_bar(dev, 2, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mm);

    /* most interrupts are not yet emulated but MacOS needs at least VBlank */
    dev->config[PCI_INTERRUPT_PIN] = 1;
    timer_init_ns(&s->vblank_timer, QEMU_CLOCK_VIRTUAL, ati_vga_vblank_irq, s);
}