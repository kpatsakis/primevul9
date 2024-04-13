static int copy_context_table(struct intel_iommu *iommu,
			      struct root_entry *old_re,
			      struct context_entry **tbl,
			      int bus, bool ext)
{
	int tbl_idx, pos = 0, idx, devfn, ret = 0, did;
	struct context_entry *new_ce = NULL, ce;
	struct context_entry *old_ce = NULL;
	struct root_entry re;
	phys_addr_t old_ce_phys;

	tbl_idx = ext ? bus * 2 : bus;
	memcpy(&re, old_re, sizeof(re));

	for (devfn = 0; devfn < 256; devfn++) {
		/* First calculate the correct index */
		idx = (ext ? devfn * 2 : devfn) % 256;

		if (idx == 0) {
			/* First save what we may have and clean up */
			if (new_ce) {
				tbl[tbl_idx] = new_ce;
				__iommu_flush_cache(iommu, new_ce,
						    VTD_PAGE_SIZE);
				pos = 1;
			}

			if (old_ce)
				memunmap(old_ce);

			ret = 0;
			if (devfn < 0x80)
				old_ce_phys = root_entry_lctp(&re);
			else
				old_ce_phys = root_entry_uctp(&re);

			if (!old_ce_phys) {
				if (ext && devfn == 0) {
					/* No LCTP, try UCTP */
					devfn = 0x7f;
					continue;
				} else {
					goto out;
				}
			}

			ret = -ENOMEM;
			old_ce = memremap(old_ce_phys, PAGE_SIZE,
					MEMREMAP_WB);
			if (!old_ce)
				goto out;

			new_ce = alloc_pgtable_page(iommu->node);
			if (!new_ce)
				goto out_unmap;

			ret = 0;
		}

		/* Now copy the context entry */
		memcpy(&ce, old_ce + idx, sizeof(ce));

		if (!__context_present(&ce))
			continue;

		did = context_domain_id(&ce);
		if (did >= 0 && did < cap_ndoms(iommu->cap))
			set_bit(did, iommu->domain_ids);

		/*
		 * We need a marker for copied context entries. This
		 * marker needs to work for the old format as well as
		 * for extended context entries.
		 *
		 * Bit 67 of the context entry is used. In the old
		 * format this bit is available to software, in the
		 * extended format it is the PGE bit, but PGE is ignored
		 * by HW if PASIDs are disabled (and thus still
		 * available).
		 *
		 * So disable PASIDs first and then mark the entry
		 * copied. This means that we don't copy PASID
		 * translations from the old kernel, but this is fine as
		 * faults there are not fatal.
		 */
		context_clear_pasid_enable(&ce);
		context_set_copied(&ce);

		new_ce[idx] = ce;
	}

	tbl[tbl_idx + pos] = new_ce;

	__iommu_flush_cache(iommu, new_ce, VTD_PAGE_SIZE);

out_unmap:
	memunmap(old_ce);

out:
	return ret;
}