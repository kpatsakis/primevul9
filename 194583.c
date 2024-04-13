static bool qxl_get_check_slot_offset(PCIQXLDevice *qxl, QXLPHYSICAL pqxl,
                                      uint32_t *s, uint64_t *o)
{
    uint64_t phys   = le64_to_cpu(pqxl);
    uint32_t slot   = (phys >> (64 -  8)) & 0xff;
    uint64_t offset = phys & 0xffffffffffff;

    if (slot >= NUM_MEMSLOTS) {
        qxl_set_guest_bug(qxl, "slot too large %d >= %d", slot,
                          NUM_MEMSLOTS);
        return false;
    }
    if (!qxl->guest_slots[slot].active) {
        qxl_set_guest_bug(qxl, "inactive slot %d\n", slot);
        return false;
    }
    if (offset < qxl->guest_slots[slot].delta) {
        qxl_set_guest_bug(qxl,
                          "slot %d offset %"PRIu64" < delta %"PRIu64"\n",
                          slot, offset, qxl->guest_slots[slot].delta);
        return false;
    }
    offset -= qxl->guest_slots[slot].delta;
    if (offset > qxl->guest_slots[slot].size) {
        qxl_set_guest_bug(qxl,
                          "slot %d offset %"PRIu64" > size %"PRIu64"\n",
                          slot, offset, qxl->guest_slots[slot].size);
        return false;
    }

    *s = slot;
    *o = offset;
    return true;
}