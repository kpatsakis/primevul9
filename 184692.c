MemoryRegion *address_space_translate(AddressSpace *as, hwaddr addr,
                                      hwaddr *xlat, hwaddr *plen,
                                      bool is_write)
{
    IOMMUTLBEntry iotlb;
    MemoryRegionSection *section;
    MemoryRegion *mr;

    for (;;) {
        AddressSpaceDispatch *d = atomic_rcu_read(&as->dispatch);
        section = address_space_translate_internal(d, addr, &addr, plen, true);
        mr = section->mr;

        if (!mr->iommu_ops) {
            break;
        }

        iotlb = mr->iommu_ops->translate(mr, addr, is_write);
        addr = ((iotlb.translated_addr & ~iotlb.addr_mask)
                | (addr & iotlb.addr_mask));
        *plen = MIN(*plen, (addr | iotlb.addr_mask) - addr + 1);
        if (!(iotlb.perm & (1 << is_write))) {
            mr = &io_mem_unassigned;
            break;
        }

        as = iotlb.target_as;
    }

    if (xen_enabled() && memory_access_is_direct(mr, is_write)) {
        hwaddr page = ((addr & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE) - addr;
        *plen = MIN(page, *plen);
    }

    *xlat = addr;
    return mr;
}