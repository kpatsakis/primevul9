static subpage_t *subpage_init(AddressSpace *as, hwaddr base)
{
    subpage_t *mmio;

    mmio = g_malloc0(sizeof(subpage_t));

    mmio->as = as;
    mmio->base = base;
    memory_region_init_io(&mmio->iomem, NULL, &subpage_ops, mmio,
                          NULL, TARGET_PAGE_SIZE);
    mmio->iomem.subpage = true;
#if defined(DEBUG_SUBPAGE)
    printf("%s: %p base " TARGET_FMT_plx " len %08x\n", __func__,
           mmio, base, TARGET_PAGE_SIZE);
#endif
    subpage_register(mmio, 0, TARGET_PAGE_SIZE-1, PHYS_SECTION_UNASSIGNED);

    return mmio;
}