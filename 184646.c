void *address_space_map(AddressSpace *as,
                        hwaddr addr,
                        hwaddr *plen,
                        bool is_write)
{
    hwaddr len = *plen;
    hwaddr done = 0;
    hwaddr l, xlat, base;
    MemoryRegion *mr, *this_mr;
    ram_addr_t raddr;

    if (len == 0) {
        return NULL;
    }

    l = len;
    rcu_read_lock();
    mr = address_space_translate(as, addr, &xlat, &l, is_write);

    if (!memory_access_is_direct(mr, is_write)) {
        if (atomic_xchg(&bounce.in_use, true)) {
            rcu_read_unlock();
            return NULL;
        }
        /* Avoid unbounded allocations */
        l = MIN(l, TARGET_PAGE_SIZE);
        bounce.buffer = qemu_memalign(TARGET_PAGE_SIZE, l);
        bounce.addr = addr;
        bounce.len = l;

        memory_region_ref(mr);
        bounce.mr = mr;
        if (!is_write) {
            address_space_read(as, addr, MEMTXATTRS_UNSPECIFIED,
                               bounce.buffer, l);
        }

        rcu_read_unlock();
        *plen = l;
        return bounce.buffer;
    }

    base = xlat;
    raddr = memory_region_get_ram_addr(mr);

    for (;;) {
        len -= l;
        addr += l;
        done += l;
        if (len == 0) {
            break;
        }

        l = len;
        this_mr = address_space_translate(as, addr, &xlat, &l, is_write);
        if (this_mr != mr || xlat != base + done) {
            break;
        }
    }

    memory_region_ref(mr);
    rcu_read_unlock();
    *plen = done;
    return qemu_ram_ptr_length(raddr + base, plen);
}