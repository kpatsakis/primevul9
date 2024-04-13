static void qxl_del_memslot(PCIQXLDevice *d, uint32_t slot_id)
{
    qemu_spice_del_memslot(&d->ssd, MEMSLOT_GROUP_HOST, slot_id);
    d->guest_slots[slot_id].active = 0;
}