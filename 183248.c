static struct dma_pte *dma_pfn_level_pte(struct dmar_domain *domain,
					 unsigned long pfn,
					 int level, int *large_page)
{
	struct dma_pte *parent, *pte;
	int total = agaw_to_level(domain->agaw);
	int offset;

	parent = domain->pgd;
	while (level <= total) {
		offset = pfn_level_offset(pfn, total);
		pte = &parent[offset];
		if (level == total)
			return pte;

		if (!dma_pte_present(pte)) {
			*large_page = total;
			break;
		}

		if (dma_pte_superpage(pte)) {
			*large_page = total;
			return pte;
		}

		parent = phys_to_virt(dma_pte_addr(pte));
		total--;
	}
	return NULL;
}