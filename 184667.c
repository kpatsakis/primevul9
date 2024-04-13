static inline uint32_t address_space_lduw_internal(AddressSpace *as,
                                                   hwaddr addr,
                                                   MemTxAttrs attrs,
                                                   MemTxResult *result,
                                                   enum device_endian endian)
{
    uint8_t *ptr;
    uint64_t val;
    MemoryRegion *mr;
    hwaddr l = 2;
    hwaddr addr1;
    MemTxResult r;

    rcu_read_lock();
    mr = address_space_translate(as, addr, &addr1, &l,
                                 false);
    if (l < 2 || !memory_access_is_direct(mr, false)) {
        /* I/O case */
        r = memory_region_dispatch_read(mr, addr1, &val, 2, attrs);
#if defined(TARGET_WORDS_BIGENDIAN)
        if (endian == DEVICE_LITTLE_ENDIAN) {
            val = bswap16(val);
        }
#else
        if (endian == DEVICE_BIG_ENDIAN) {
            val = bswap16(val);
        }
#endif
    } else {
        /* RAM case */
        ptr = qemu_get_ram_ptr((memory_region_get_ram_addr(mr)
                                & TARGET_PAGE_MASK)
                               + addr1);
        switch (endian) {
        case DEVICE_LITTLE_ENDIAN:
            val = lduw_le_p(ptr);
            break;
        case DEVICE_BIG_ENDIAN:
            val = lduw_be_p(ptr);
            break;
        default:
            val = lduw_p(ptr);
            break;
        }
        r = MEMTX_OK;
    }
    if (result) {
        *result = r;
    }
    rcu_read_unlock();
    return val;
}