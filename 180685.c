static int intel_iommu_map(struct iommu_domain *domain,
			   unsigned long iova, phys_addr_t hpa,
			   size_t size, int iommu_prot)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	u64 max_addr;
	int prot = 0;
	int ret;

	if (iommu_prot & IOMMU_READ)
		prot |= DMA_PTE_READ;
	if (iommu_prot & IOMMU_WRITE)
		prot |= DMA_PTE_WRITE;
	if ((iommu_prot & IOMMU_CACHE) && dmar_domain->iommu_snooping)
		prot |= DMA_PTE_SNP;

	max_addr = iova + size;
	if (dmar_domain->max_addr < max_addr) {
		u64 end;

		/* check if minimum agaw is sufficient for mapped address */
		end = __DOMAIN_MAX_ADDR(dmar_domain->gaw) + 1;
		if (end < max_addr) {
			pr_err("%s: iommu width (%d) is not "
			       "sufficient for the mapped address (%llx)\n",
			       __func__, dmar_domain->gaw, max_addr);
			return -EFAULT;
		}
		dmar_domain->max_addr = max_addr;
	}
	/* Round up size to next multiple of PAGE_SIZE, if it and
	   the low bits of hpa would take us onto the next page */
	size = aligned_nrpages(hpa, size);
	ret = domain_pfn_mapping(dmar_domain, iova >> VTD_PAGE_SHIFT,
				 hpa >> VTD_PAGE_SHIFT, size, prot);
	return ret;
}