address_space_translate_for_iotlb(CPUState *cpu, hwaddr addr,
                                  hwaddr *xlat, hwaddr *plen)
{
    MemoryRegionSection *section;
    section = address_space_translate_internal(cpu->memory_dispatch,
                                               addr, xlat, plen, false);

    assert(!section->mr->iommu_ops);
    return section;
}