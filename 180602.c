static int __init init_dmars(void)
{
	struct dmar_drhd_unit *drhd;
	struct dmar_rmrr_unit *rmrr;
	bool copied_tables = false;
	struct device *dev;
	struct intel_iommu *iommu;
	int i, ret;

	/*
	 * for each drhd
	 *    allocate root
	 *    initialize and program root entry to not present
	 * endfor
	 */
	for_each_drhd_unit(drhd) {
		/*
		 * lock not needed as this is only incremented in the single
		 * threaded kernel __init code path all other access are read
		 * only
		 */
		if (g_num_of_iommus < DMAR_UNITS_SUPPORTED) {
			g_num_of_iommus++;
			continue;
		}
		pr_err_once("Exceeded %d IOMMUs\n", DMAR_UNITS_SUPPORTED);
	}

	/* Preallocate enough resources for IOMMU hot-addition */
	if (g_num_of_iommus < DMAR_UNITS_SUPPORTED)
		g_num_of_iommus = DMAR_UNITS_SUPPORTED;

	g_iommus = kcalloc(g_num_of_iommus, sizeof(struct intel_iommu *),
			GFP_KERNEL);
	if (!g_iommus) {
		pr_err("Allocating global iommu array failed\n");
		ret = -ENOMEM;
		goto error;
	}

	for_each_active_iommu(iommu, drhd) {
		/*
		 * Find the max pasid size of all IOMMU's in the system.
		 * We need to ensure the system pasid table is no bigger
		 * than the smallest supported.
		 */
		if (pasid_enabled(iommu)) {
			u32 temp = 2 << ecap_pss(iommu->ecap);

			intel_pasid_max_id = min_t(u32, temp,
						   intel_pasid_max_id);
		}

		g_iommus[iommu->seq_id] = iommu;

		intel_iommu_init_qi(iommu);

		ret = iommu_init_domains(iommu);
		if (ret)
			goto free_iommu;

		init_translation_status(iommu);

		if (translation_pre_enabled(iommu) && !is_kdump_kernel()) {
			iommu_disable_translation(iommu);
			clear_translation_pre_enabled(iommu);
			pr_warn("Translation was enabled for %s but we are not in kdump mode\n",
				iommu->name);
		}

		/*
		 * TBD:
		 * we could share the same root & context tables
		 * among all IOMMU's. Need to Split it later.
		 */
		ret = iommu_alloc_root_entry(iommu);
		if (ret)
			goto free_iommu;

		if (translation_pre_enabled(iommu)) {
			pr_info("Translation already enabled - trying to copy translation structures\n");

			ret = copy_translation_tables(iommu);
			if (ret) {
				/*
				 * We found the IOMMU with translation
				 * enabled - but failed to copy over the
				 * old root-entry table. Try to proceed
				 * by disabling translation now and
				 * allocating a clean root-entry table.
				 * This might cause DMAR faults, but
				 * probably the dump will still succeed.
				 */
				pr_err("Failed to copy translation tables from previous kernel for %s\n",
				       iommu->name);
				iommu_disable_translation(iommu);
				clear_translation_pre_enabled(iommu);
			} else {
				pr_info("Copied translation tables from previous kernel for %s\n",
					iommu->name);
				copied_tables = true;
			}
		}

		if (!ecap_pass_through(iommu->ecap))
			hw_pass_through = 0;
#ifdef CONFIG_INTEL_IOMMU_SVM
		if (pasid_enabled(iommu))
			intel_svm_init(iommu);
#endif
	}

	/*
	 * Now that qi is enabled on all iommus, set the root entry and flush
	 * caches. This is required on some Intel X58 chipsets, otherwise the
	 * flush_context function will loop forever and the boot hangs.
	 */
	for_each_active_iommu(iommu, drhd) {
		iommu_flush_write_buffer(iommu);
		iommu_set_root_entry(iommu);
		iommu->flush.flush_context(iommu, 0, 0, 0, DMA_CCMD_GLOBAL_INVL);
		iommu->flush.flush_iotlb(iommu, 0, 0, 0, DMA_TLB_GLOBAL_FLUSH);
	}

	if (iommu_pass_through)
		iommu_identity_mapping |= IDENTMAP_ALL;

#ifdef CONFIG_INTEL_IOMMU_BROKEN_GFX_WA
	iommu_identity_mapping |= IDENTMAP_GFX;
#endif

	check_tylersburg_isoch();

	if (iommu_identity_mapping) {
		ret = si_domain_init(hw_pass_through);
		if (ret)
			goto free_iommu;
	}


	/*
	 * If we copied translations from a previous kernel in the kdump
	 * case, we can not assign the devices to domains now, as that
	 * would eliminate the old mappings. So skip this part and defer
	 * the assignment to device driver initialization time.
	 */
	if (copied_tables)
		goto domains_done;

	/*
	 * If pass through is not set or not enabled, setup context entries for
	 * identity mappings for rmrr, gfx, and isa and may fall back to static
	 * identity mapping if iommu_identity_mapping is set.
	 */
	if (iommu_identity_mapping) {
		ret = iommu_prepare_static_identity_mapping(hw_pass_through);
		if (ret) {
			pr_crit("Failed to setup IOMMU pass-through\n");
			goto free_iommu;
		}
	}
	/*
	 * For each rmrr
	 *   for each dev attached to rmrr
	 *   do
	 *     locate drhd for dev, alloc domain for dev
	 *     allocate free domain
	 *     allocate page table entries for rmrr
	 *     if context not allocated for bus
	 *           allocate and init context
	 *           set present in root table for this bus
	 *     init context with domain, translation etc
	 *    endfor
	 * endfor
	 */
	pr_info("Setting RMRR:\n");
	for_each_rmrr_units(rmrr) {
		/* some BIOS lists non-exist devices in DMAR table. */
		for_each_active_dev_scope(rmrr->devices, rmrr->devices_cnt,
					  i, dev) {
			ret = iommu_prepare_rmrr_dev(rmrr, dev);
			if (ret)
				pr_err("Mapping reserved region failed\n");
		}
	}

	iommu_prepare_isa();

domains_done:

	/*
	 * for each drhd
	 *   enable fault log
	 *   global invalidate context cache
	 *   global invalidate iotlb
	 *   enable translation
	 */
	for_each_iommu(iommu, drhd) {
		if (drhd->ignored) {
			/*
			 * we always have to disable PMRs or DMA may fail on
			 * this device
			 */
			if (force_on)
				iommu_disable_protect_mem_regions(iommu);
			continue;
		}

		iommu_flush_write_buffer(iommu);

#ifdef CONFIG_INTEL_IOMMU_SVM
		if (pasid_enabled(iommu) && ecap_prs(iommu->ecap)) {
			ret = intel_svm_enable_prq(iommu);
			if (ret)
				goto free_iommu;
		}
#endif
		ret = dmar_set_interrupt(iommu);
		if (ret)
			goto free_iommu;

		if (!translation_pre_enabled(iommu))
			iommu_enable_translation(iommu);

		iommu_disable_protect_mem_regions(iommu);
	}

	return 0;

free_iommu:
	for_each_active_iommu(iommu, drhd) {
		disable_dmar_iommu(iommu);
		free_dmar_iommu(iommu);
	}

	kfree(g_iommus);

error:
	return ret;
}