MemTxResult address_space_rw(AddressSpace *as, hwaddr addr, MemTxAttrs attrs,
                             uint8_t *buf, int len, bool is_write)
{
    hwaddr l;
    uint8_t *ptr;
    uint64_t val;
    hwaddr addr1;
    MemoryRegion *mr;
    MemTxResult result = MEMTX_OK;

    rcu_read_lock();
    while (len > 0) {
        l = len;
        mr = address_space_translate(as, addr, &addr1, &l, is_write);

        if (is_write) {
            if (!memory_access_is_direct(mr, is_write)) {
                l = memory_access_size(mr, l, addr1);
                /* XXX: could force current_cpu to NULL to avoid
                   potential bugs */
                switch (l) {
                case 8:
                    /* 64 bit write access */
                    val = ldq_p(buf);
                    result |= memory_region_dispatch_write(mr, addr1, val, 8,
                                                           attrs);
                    break;
                case 4:
                    /* 32 bit write access */
                    val = ldl_p(buf);
                    result |= memory_region_dispatch_write(mr, addr1, val, 4,
                                                           attrs);
                    break;
                case 2:
                    /* 16 bit write access */
                    val = lduw_p(buf);
                    result |= memory_region_dispatch_write(mr, addr1, val, 2,
                                                           attrs);
                    break;
                case 1:
                    /* 8 bit write access */
                    val = ldub_p(buf);
                    result |= memory_region_dispatch_write(mr, addr1, val, 1,
                                                           attrs);
                    break;
                default:
                    abort();
                }
            } else {
                addr1 += memory_region_get_ram_addr(mr);
                /* RAM case */
                ptr = qemu_get_ram_ptr(addr1);
                memcpy(ptr, buf, l);
                invalidate_and_set_dirty(mr, addr1, l);
            }
        } else {
            if (!memory_access_is_direct(mr, is_write)) {
                /* I/O case */
                l = memory_access_size(mr, l, addr1);
                switch (l) {
                case 8:
                    /* 64 bit read access */
                    result |= memory_region_dispatch_read(mr, addr1, &val, 8,
                                                          attrs);
                    stq_p(buf, val);
                    break;
                case 4:
                    /* 32 bit read access */
                    result |= memory_region_dispatch_read(mr, addr1, &val, 4,
                                                          attrs);
                    stl_p(buf, val);
                    break;
                case 2:
                    /* 16 bit read access */
                    result |= memory_region_dispatch_read(mr, addr1, &val, 2,
                                                          attrs);
                    stw_p(buf, val);
                    break;
                case 1:
                    /* 8 bit read access */
                    result |= memory_region_dispatch_read(mr, addr1, &val, 1,
                                                          attrs);
                    stb_p(buf, val);
                    break;
                default:
                    abort();
                }
            } else {
                /* RAM case */
                ptr = qemu_get_ram_ptr(mr->ram_addr + addr1);
                memcpy(buf, ptr, l);
            }
        }
        len -= l;
        buf += l;
        addr += l;
    }
    rcu_read_unlock();

    return result;
}