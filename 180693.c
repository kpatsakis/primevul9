static inline int hardware_largepage_caps(struct dmar_domain *domain,
					  unsigned long iov_pfn,
					  unsigned long phy_pfn,
					  unsigned long pages)
{
	int support, level = 1;
	unsigned long pfnmerge;

	support = domain->iommu_superpage;

	/* To use a large page, the virtual *and* physical addresses
	   must be aligned to 2MiB/1GiB/etc. Lower bits set in either
	   of them will mean we have to use smaller pages. So just
	   merge them and check both at once. */
	pfnmerge = iov_pfn | phy_pfn;

	while (support && !(pfnmerge & ~VTD_STRIDE_MASK)) {
		pages >>= VTD_STRIDE_SHIFT;
		if (!pages)
			break;
		pfnmerge >>= VTD_STRIDE_SHIFT;
		level++;
		support--;
	}
	return level;
}