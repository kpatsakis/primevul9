static void qxl_create_memslots(PCIQXLDevice *d)
{
    int i;

    for (i = 0; i < NUM_MEMSLOTS; i++) {
        if (!d->guest_slots[i].active) {
            continue;
        }
        qxl_add_memslot(d, i, 0, QXL_SYNC);
    }
}