MemTxResult memory_region_dispatch_read(struct uc_struct *uc, MemoryRegion *mr,
                                        hwaddr addr,
                                        uint64_t *pval,
                                        MemOp op,
                                        MemTxAttrs attrs)
{
    unsigned size = memop_size(op);
    MemTxResult r;

    if (!memory_region_access_valid(uc, mr, addr, size, false, attrs)) {
        *pval = unassigned_mem_read(mr, addr, size);
        return MEMTX_DECODE_ERROR;
    }

    r = memory_region_dispatch_read1(uc, mr, addr, pval, size, attrs);
    adjust_endianness(mr, pval, op);
    return r;
}