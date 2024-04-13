static inline void address_space_stl_internal(AddressSpace *as,
                                              hwaddr addr, uint32_t val,
                                              MemTxAttrs attrs,
                                              MemTxResult *result,
                                              enum device_endian endian)
{
    uint8_t *ptr;
    MemoryRegion *mr;
    hwaddr l = 4;
    hwaddr addr1;
    MemTxResult r;

    rcu_read_lock();
    mr = address_space_translate(as, addr, &addr1, &l,
                                 true);
    if (l < 4 || !memory_access_is_direct(mr, true)) {
#if defined(TARGET_WORDS_BIGENDIAN)
        if (endian == DEVICE_LITTLE_ENDIAN) {
            val = bswap32(val);
        }
#else
        if (endian == DEVICE_BIG_ENDIAN) {
            val = bswap32(val);
        }
#endif
        r = memory_region_dispatch_write(mr, addr1, val, 4, attrs);
    } else {
        /* RAM case */
        addr1 += memory_region_get_ram_addr(mr) & TARGET_PAGE_MASK;
        ptr = qemu_get_ram_ptr(addr1);
        switch (endian) {
        case DEVICE_LITTLE_ENDIAN:
            stl_le_p(ptr, val);
            break;
        case DEVICE_BIG_ENDIAN:
            stl_be_p(ptr, val);
            break;
        default:
            stl_p(ptr, val);
            break;
        }
        invalidate_and_set_dirty(mr, addr1, 4);
        r = MEMTX_OK;
    }
    if (result) {
        *result = r;
    }
    rcu_read_unlock();
}