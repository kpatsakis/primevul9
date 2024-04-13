static MemoryRegionSection address_space_translate_iommu(IOMMUMemoryRegion *iommu_mr,
                                                         hwaddr *xlat,
                                                         hwaddr *plen_out,
                                                         hwaddr *page_mask_out,
                                                         bool is_write,
                                                         bool is_mmio,
                                                         AddressSpace **target_as,
                                                         MemTxAttrs attrs)
{
    MemoryRegionSection *section;
    hwaddr page_mask = (hwaddr)-1;
    MemoryRegion *mr = MEMORY_REGION(iommu_mr);

    do {
        hwaddr addr = *xlat;
        IOMMUMemoryRegionClass *imrc = memory_region_get_iommu_class_nocheck(iommu_mr);
        int iommu_idx = 0;
        IOMMUTLBEntry iotlb;

        if (imrc->attrs_to_index) {
            iommu_idx = imrc->attrs_to_index(iommu_mr, attrs);
        }

        iotlb = imrc->translate(iommu_mr, addr, is_write ?
                                IOMMU_WO : IOMMU_RO, iommu_idx);

        if (!(iotlb.perm & (1 << is_write))) {
            goto unassigned;
        }

        addr = ((iotlb.translated_addr & ~iotlb.addr_mask)
                | (addr & iotlb.addr_mask));
        page_mask &= iotlb.addr_mask;
        *plen_out = MIN(*plen_out, (addr | iotlb.addr_mask) - addr + 1);
        *target_as = iotlb.target_as;

        section = address_space_translate_internal(
                address_space_to_dispatch(iotlb.target_as), addr, xlat,
                plen_out, is_mmio);

        iommu_mr = memory_region_get_iommu(section->mr);
    } while (unlikely(iommu_mr));

    if (page_mask_out) {
        *page_mask_out = page_mask;
    }
    return *section;

unassigned:
    return (MemoryRegionSection) { .mr = &(mr->uc->io_mem_unassigned) };
}