static void qxl_dirty_one_surface(PCIQXLDevice *qxl, QXLPHYSICAL pqxl,
                                  uint32_t height, int32_t stride)
{
    uint64_t offset, size;
    uint32_t slot;
    bool rc;

    rc = qxl_get_check_slot_offset(qxl, pqxl, &slot, &offset);
    assert(rc == true);
    size = (uint64_t)height * abs(stride);
    trace_qxl_surfaces_dirty(qxl->id, offset, size);
    qxl_set_dirty(qxl->guest_slots[slot].mr,
                  qxl->guest_slots[slot].offset + offset,
                  qxl->guest_slots[slot].offset + offset + size);
}