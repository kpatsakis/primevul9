static void qxl_reset_state(PCIQXLDevice *d)
{
    QXLRom *rom = d->rom;

    qxl_check_state(d);
    d->shadow_rom.update_id = cpu_to_le32(0);
    *rom = d->shadow_rom;
    qxl_rom_set_dirty(d);
    init_qxl_ram(d);
    d->num_free_res = 0;
    d->last_release = NULL;
    memset(&d->ssd.dirty, 0, sizeof(d->ssd.dirty));
    qxl_update_irq(d);
}