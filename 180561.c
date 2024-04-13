static int __domain_mapping(struct dmar_domain *domain, unsigned long iov_pfn,
			    struct scatterlist *sg, unsigned long phys_pfn,
			    unsigned long nr_pages, int prot)
{
	struct dma_pte *first_pte = NULL, *pte = NULL;
	phys_addr_t uninitialized_var(pteval);
	unsigned long sg_res = 0;
	unsigned int largepage_lvl = 0;
	unsigned long lvl_pages = 0;

	BUG_ON(!domain_pfn_supported(domain, iov_pfn + nr_pages - 1));

	if ((prot & (DMA_PTE_READ|DMA_PTE_WRITE)) == 0)
		return -EINVAL;

	prot &= DMA_PTE_READ | DMA_PTE_WRITE | DMA_PTE_SNP;

	if (!sg) {
		sg_res = nr_pages;
		pteval = ((phys_addr_t)phys_pfn << VTD_PAGE_SHIFT) | prot;
	}

	while (nr_pages > 0) {
		uint64_t tmp;

		if (!sg_res) {
			unsigned int pgoff = sg->offset & ~PAGE_MASK;

			sg_res = aligned_nrpages(sg->offset, sg->length);
			sg->dma_address = ((dma_addr_t)iov_pfn << VTD_PAGE_SHIFT) + pgoff;
			sg->dma_length = sg->length;
			pteval = (sg_phys(sg) - pgoff) | prot;
			phys_pfn = pteval >> VTD_PAGE_SHIFT;
		}

		if (!pte) {
			largepage_lvl = hardware_largepage_caps(domain, iov_pfn, phys_pfn, sg_res);

			first_pte = pte = pfn_to_dma_pte(domain, iov_pfn, &largepage_lvl);
			if (!pte)
				return -ENOMEM;
			/* It is large page*/
			if (largepage_lvl > 1) {
				unsigned long nr_superpages, end_pfn;

				pteval |= DMA_PTE_LARGE_PAGE;
				lvl_pages = lvl_to_nr_pages(largepage_lvl);

				nr_superpages = sg_res / lvl_pages;
				end_pfn = iov_pfn + nr_superpages * lvl_pages - 1;

				/*
				 * Ensure that old small page tables are
				 * removed to make room for superpage(s).
				 * We're adding new large pages, so make sure
				 * we don't remove their parent tables.
				 */
				dma_pte_free_pagetable(domain, iov_pfn, end_pfn,
						       largepage_lvl + 1);
			} else {
				pteval &= ~(uint64_t)DMA_PTE_LARGE_PAGE;
			}

		}
		/* We don't need lock here, nobody else
		 * touches the iova range
		 */
		tmp = cmpxchg64_local(&pte->val, 0ULL, pteval);
		if (tmp) {
			static int dumps = 5;
			pr_crit("ERROR: DMA PTE for vPFN 0x%lx already set (to %llx not %llx)\n",
				iov_pfn, tmp, (unsigned long long)pteval);
			if (dumps) {
				dumps--;
				debug_dma_dump_mappings(NULL);
			}
			WARN_ON(1);
		}

		lvl_pages = lvl_to_nr_pages(largepage_lvl);

		BUG_ON(nr_pages < lvl_pages);
		BUG_ON(sg_res < lvl_pages);

		nr_pages -= lvl_pages;
		iov_pfn += lvl_pages;
		phys_pfn += lvl_pages;
		pteval += lvl_pages * VTD_PAGE_SIZE;
		sg_res -= lvl_pages;

		/* If the next PTE would be the first in a new page, then we
		   need to flush the cache on the entries we've just written.
		   And then we'll need to recalculate 'pte', so clear it and
		   let it get set again in the if (!pte) block above.

		   If we're done (!nr_pages) we need to flush the cache too.

		   Also if we've been setting superpages, we may need to
		   recalculate 'pte' and switch back to smaller pages for the
		   end of the mapping, if the trailing size is not enough to
		   use another superpage (i.e. sg_res < lvl_pages). */
		pte++;
		if (!nr_pages || first_pte_in_page(pte) ||
		    (largepage_lvl > 1 && sg_res < lvl_pages)) {
			domain_flush_cache(domain, first_pte,
					   (void *)pte - (void *)first_pte);
			pte = NULL;
		}

		if (!sg_res && nr_pages)
			sg = sg_next(sg);
	}
	return 0;
}