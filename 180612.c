static int copy_translation_tables(struct intel_iommu *iommu)
{
	struct context_entry **ctxt_tbls;
	struct root_entry *old_rt;
	phys_addr_t old_rt_phys;
	int ctxt_table_entries;
	unsigned long flags;
	u64 rtaddr_reg;
	int bus, ret;
	bool new_ext, ext;

	rtaddr_reg = dmar_readq(iommu->reg + DMAR_RTADDR_REG);
	ext        = !!(rtaddr_reg & DMA_RTADDR_RTT);
	new_ext    = !!ecap_ecs(iommu->ecap);

	/*
	 * The RTT bit can only be changed when translation is disabled,
	 * but disabling translation means to open a window for data
	 * corruption. So bail out and don't copy anything if we would
	 * have to change the bit.
	 */
	if (new_ext != ext)
		return -EINVAL;

	old_rt_phys = rtaddr_reg & VTD_PAGE_MASK;
	if (!old_rt_phys)
		return -EINVAL;

	old_rt = memremap(old_rt_phys, PAGE_SIZE, MEMREMAP_WB);
	if (!old_rt)
		return -ENOMEM;

	/* This is too big for the stack - allocate it from slab */
	ctxt_table_entries = ext ? 512 : 256;
	ret = -ENOMEM;
	ctxt_tbls = kcalloc(ctxt_table_entries, sizeof(void *), GFP_KERNEL);
	if (!ctxt_tbls)
		goto out_unmap;

	for (bus = 0; bus < 256; bus++) {
		ret = copy_context_table(iommu, &old_rt[bus],
					 ctxt_tbls, bus, ext);
		if (ret) {
			pr_err("%s: Failed to copy context table for bus %d\n",
				iommu->name, bus);
			continue;
		}
	}

	spin_lock_irqsave(&iommu->lock, flags);

	/* Context tables are copied, now write them to the root_entry table */
	for (bus = 0; bus < 256; bus++) {
		int idx = ext ? bus * 2 : bus;
		u64 val;

		if (ctxt_tbls[idx]) {
			val = virt_to_phys(ctxt_tbls[idx]) | 1;
			iommu->root_entry[bus].lo = val;
		}

		if (!ext || !ctxt_tbls[idx + 1])
			continue;

		val = virt_to_phys(ctxt_tbls[idx + 1]) | 1;
		iommu->root_entry[bus].hi = val;
	}

	spin_unlock_irqrestore(&iommu->lock, flags);

	kfree(ctxt_tbls);

	__iommu_flush_cache(iommu, iommu->root_entry, PAGE_SIZE);

	ret = 0;

out_unmap:
	memunmap(old_rt);

	return ret;
}