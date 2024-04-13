MemoryRegion *iotlb_to_region(CPUState *cpu, hwaddr index)
{
    AddressSpaceDispatch *d = atomic_rcu_read(&cpu->memory_dispatch);
    MemoryRegionSection *sections = d->map.sections;

    return sections[index & ~TARGET_PAGE_MASK].mr;
}