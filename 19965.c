MemoryRegionSection *iotlb_to_section(CPUState *cpu,
                                      hwaddr index, MemTxAttrs attrs)
{
#ifdef TARGET_ARM
    struct uc_struct *uc = cpu->uc;
#endif
    int asidx = cpu_asidx_from_attrs(cpu, attrs);
    CPUAddressSpace *cpuas = &cpu->cpu_ases[asidx];
    AddressSpaceDispatch *d = cpuas->memory_dispatch;
    MemoryRegionSection *sections = d->map.sections;

    return &sections[index & ~TARGET_PAGE_MASK];
}