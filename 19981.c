void *address_space_map(AddressSpace *as,
                        hwaddr addr,
                        hwaddr *plen,
                        bool is_write,
                        MemTxAttrs attrs)
{
    hwaddr len = *plen;
    hwaddr l, xlat;
    MemoryRegion *mr;
    void *ptr;
    FlatView *fv;
    struct uc_struct *uc = as->uc;

    if (len == 0) {
        return NULL;
    }

    l = len;
    fv = address_space_to_flatview(as);
    mr = flatview_translate(uc, fv, addr, &xlat, &l, is_write, attrs);

    if (!memory_access_is_direct(mr, is_write)) {
        /* Avoid unbounded allocations */
        l = MIN(l, TARGET_PAGE_SIZE);
        mr->uc->bounce.buffer = qemu_memalign(TARGET_PAGE_SIZE, l);
        mr->uc->bounce.addr = addr;
        mr->uc->bounce.len = l;

        mr->uc->bounce.mr = mr;
        if (!is_write) {
            flatview_read(as->uc, fv, addr, MEMTXATTRS_UNSPECIFIED,
                               mr->uc->bounce.buffer, l);
        }

        *plen = l;
        return mr->uc->bounce.buffer;
    }


    *plen = flatview_extend_translation(as->uc, fv, addr, len, mr, xlat,
                                        l, is_write, attrs);
    ptr = qemu_ram_ptr_length(as->uc, mr->ram_block, xlat, plen, true);

    return ptr;
}