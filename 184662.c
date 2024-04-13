void address_space_stl_notdirty(AddressSpace *as, hwaddr addr, uint32_t val,
                                MemTxAttrs attrs, MemTxResult *result)
{
    uint8_t *ptr;
    MemoryRegion *mr;
    hwaddr l = 4;
    hwaddr addr1;
    MemTxResult r;
    uint8_t dirty_log_mask;

    rcu_read_lock();
    mr = address_space_translate(as, addr, &addr1, &l,
                                 true);
    if (l < 4 || !memory_access_is_direct(mr, true)) {
        r = memory_region_dispatch_write(mr, addr1, val, 4, attrs);
    } else {
        addr1 += memory_region_get_ram_addr(mr) & TARGET_PAGE_MASK;
        ptr = qemu_get_ram_ptr(addr1);
        stl_p(ptr, val);

        dirty_log_mask = memory_region_get_dirty_log_mask(mr);
        dirty_log_mask &= ~(1 << DIRTY_MEMORY_CODE);
        cpu_physical_memory_set_dirty_range(addr1, 4, dirty_log_mask);
        r = MEMTX_OK;
    }
    if (result) {
        *result = r;
    }
    rcu_read_unlock();
}