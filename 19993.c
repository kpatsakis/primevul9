static MemTxResult memory_region_dispatch_read1(struct uc_struct *uc, MemoryRegion *mr,
                                                hwaddr addr,
                                                uint64_t *pval,
                                                unsigned size,
                                                MemTxAttrs attrs)
{
    *pval = 0;

    if (mr->ops->read) {
        return access_with_adjusted_size(uc, addr, pval, size,
                                         mr->ops->impl.min_access_size,
                                         mr->ops->impl.max_access_size,
                                         memory_region_read_accessor,
                                         mr, attrs);
    } else {
        return access_with_adjusted_size(uc, addr, pval, size,
                                         mr->ops->impl.min_access_size,
                                         mr->ops->impl.max_access_size,
                                         memory_region_read_with_attrs_accessor,
                                         mr, attrs);
    }
}