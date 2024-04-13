void *qxl_phys2virt(PCIQXLDevice *qxl, QXLPHYSICAL pqxl, int group_id)
{
    uint64_t offset;
    uint32_t slot;
    void *ptr;

    switch (group_id) {
    case MEMSLOT_GROUP_HOST:
        offset = le64_to_cpu(pqxl) & 0xffffffffffff;
        return (void *)(intptr_t)offset;
    case MEMSLOT_GROUP_GUEST:
        if (!qxl_get_check_slot_offset(qxl, pqxl, &slot, &offset)) {
            return NULL;
        }
        ptr = memory_region_get_ram_ptr(qxl->guest_slots[slot].mr);
        ptr += qxl->guest_slots[slot].offset;
        ptr += offset;
        return ptr;
    }
    return NULL;
}