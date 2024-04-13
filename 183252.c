struct context_entry *iommu_context_addr(struct intel_iommu *iommu, u8 bus,
					 u8 devfn, int alloc)
{
	struct root_entry *root = &iommu->root_entry[bus];
	struct context_entry *context;
	u64 *entry;

	entry = &root->lo;
	if (sm_supported(iommu)) {
		if (devfn >= 0x80) {
			devfn -= 0x80;
			entry = &root->hi;
		}
		devfn *= 2;
	}
	if (*entry & 1)
		context = phys_to_virt(*entry & VTD_PAGE_MASK);
	else {
		unsigned long phy_addr;
		if (!alloc)
			return NULL;

		context = alloc_pgtable_page(iommu->node);
		if (!context)
			return NULL;

		__iommu_flush_cache(iommu, (void *)context, CONTEXT_SIZE);
		phy_addr = virt_to_phys((void *)context);
		*entry = phy_addr | 1;
		__iommu_flush_cache(iommu, entry, sizeof(*entry));
	}
	return &context[devfn];
}