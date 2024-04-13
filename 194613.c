static void qxl_reset_memslots(PCIQXLDevice *d)
{
    qxl_spice_reset_memslots(d);
    memset(&d->guest_slots, 0, sizeof(d->guest_slots));
}