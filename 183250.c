static int domain_context_mapping_one(struct dmar_domain *domain,
				      struct intel_iommu *iommu,
				      struct pasid_table *table,
				      u8 bus, u8 devfn)
{
	u16 did = domain->iommu_did[iommu->seq_id];
	int translation = CONTEXT_TT_MULTI_LEVEL;
	struct device_domain_info *info = NULL;
	struct context_entry *context;
	unsigned long flags;
	int ret;

	WARN_ON(did == 0);

	if (hw_pass_through && domain_type_is_si(domain))
		translation = CONTEXT_TT_PASS_THROUGH;

	pr_debug("Set context mapping for %02x:%02x.%d\n",
		bus, PCI_SLOT(devfn), PCI_FUNC(devfn));

	BUG_ON(!domain->pgd);

	spin_lock_irqsave(&device_domain_lock, flags);
	spin_lock(&iommu->lock);

	ret = -ENOMEM;
	context = iommu_context_addr(iommu, bus, devfn, 1);
	if (!context)
		goto out_unlock;

	ret = 0;
	if (context_present(context))
		goto out_unlock;

	/*
	 * For kdump cases, old valid entries may be cached due to the
	 * in-flight DMA and copied pgtable, but there is no unmapping
	 * behaviour for them, thus we need an explicit cache flush for
	 * the newly-mapped device. For kdump, at this point, the device
	 * is supposed to finish reset at its driver probe stage, so no
	 * in-flight DMA will exist, and we don't need to worry anymore
	 * hereafter.
	 */
	if (context_copied(context)) {
		u16 did_old = context_domain_id(context);

		if (did_old < cap_ndoms(iommu->cap)) {
			iommu->flush.flush_context(iommu, did_old,
						   (((u16)bus) << 8) | devfn,
						   DMA_CCMD_MASK_NOBIT,
						   DMA_CCMD_DEVICE_INVL);
			iommu->flush.flush_iotlb(iommu, did_old, 0, 0,
						 DMA_TLB_DSI_FLUSH);
		}
	}

	context_clear_entry(context);

	if (sm_supported(iommu)) {
		unsigned long pds;

		WARN_ON(!table);

		/* Setup the PASID DIR pointer: */
		pds = context_get_sm_pds(table);
		context->lo = (u64)virt_to_phys(table->table) |
				context_pdts(pds);

		/* Setup the RID_PASID field: */
		context_set_sm_rid2pasid(context, PASID_RID2PASID);

		/*
		 * Setup the Device-TLB enable bit and Page request
		 * Enable bit:
		 */
		info = iommu_support_dev_iotlb(domain, iommu, bus, devfn);
		if (info && info->ats_supported)
			context_set_sm_dte(context);
		if (info && info->pri_supported)
			context_set_sm_pre(context);
	} else {
		struct dma_pte *pgd = domain->pgd;
		int agaw;

		context_set_domain_id(context, did);
		context_set_translation_type(context, translation);

		if (translation != CONTEXT_TT_PASS_THROUGH) {
			/*
			 * Skip top levels of page tables for iommu which has
			 * less agaw than default. Unnecessary for PT mode.
			 */
			for (agaw = domain->agaw; agaw > iommu->agaw; agaw--) {
				ret = -ENOMEM;
				pgd = phys_to_virt(dma_pte_addr(pgd));
				if (!dma_pte_present(pgd))
					goto out_unlock;
			}

			info = iommu_support_dev_iotlb(domain, iommu, bus, devfn);
			if (info && info->ats_supported)
				translation = CONTEXT_TT_DEV_IOTLB;
			else
				translation = CONTEXT_TT_MULTI_LEVEL;

			context_set_address_root(context, virt_to_phys(pgd));
			context_set_address_width(context, agaw);
		} else {
			/*
			 * In pass through mode, AW must be programmed to
			 * indicate the largest AGAW value supported by
			 * hardware. And ASR is ignored by hardware.
			 */
			context_set_address_width(context, iommu->msagaw);
		}
	}

	context_set_fault_enable(context);
	context_set_present(context);
	domain_flush_cache(domain, context, sizeof(*context));

	/*
	 * It's a non-present to present mapping. If hardware doesn't cache
	 * non-present entry we only need to flush the write-buffer. If the
	 * _does_ cache non-present entries, then it does so in the special
	 * domain #0, which we have to flush:
	 */
	if (cap_caching_mode(iommu->cap)) {
		iommu->flush.flush_context(iommu, 0,
					   (((u16)bus) << 8) | devfn,
					   DMA_CCMD_MASK_NOBIT,
					   DMA_CCMD_DEVICE_INVL);
		iommu->flush.flush_iotlb(iommu, did, 0, 0, DMA_TLB_DSI_FLUSH);
	} else {
		iommu_flush_write_buffer(iommu);
	}
	iommu_enable_dev_iotlb(info);

	ret = 0;

out_unlock:
	spin_unlock(&iommu->lock);
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return ret;
}