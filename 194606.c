static void qxl_check_state(PCIQXLDevice *d)
{
    QXLRam *ram = d->ram;
    int spice_display_running = qemu_spice_display_is_running(&d->ssd);

    assert(!spice_display_running || SPICE_RING_IS_EMPTY(&ram->cmd_ring));
    assert(!spice_display_running || SPICE_RING_IS_EMPTY(&ram->cursor_ring));
}