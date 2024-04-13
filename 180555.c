int __init intel_iommu_init(void)
{
	int ret = -ENODEV;
	struct dmar_drhd_unit *drhd;
	struct intel_iommu *iommu;

	/*
	 * Intel IOMMU is required for a TXT/tboot launch or platform
	 * opt in, so enforce that.
	 */
	force_on = tboot_force_iommu() || platform_optin_force_iommu();

	if (iommu_init_mempool()) {
		if (force_on)
			panic("tboot: Failed to initialize iommu memory\n");
		return -ENOMEM;
	}

	down_write(&dmar_global_lock);
	if (dmar_table_init()) {
		if (force_on)
			panic("tboot: Failed to initialize DMAR table\n");
		goto out_free_dmar;
	}

	if (dmar_dev_scope_init() < 0) {
		if (force_on)
			panic("tboot: Failed to initialize DMAR device scope\n");
		goto out_free_dmar;
	}

	up_write(&dmar_global_lock);

	/*
	 * The bus notifier takes the dmar_global_lock, so lockdep will
	 * complain later when we register it under the lock.
	 */
	dmar_register_bus_notifier();

	down_write(&dmar_global_lock);

	if (no_iommu || dmar_disabled) {
		/*
		 * We exit the function here to ensure IOMMU's remapping and
		 * mempool aren't setup, which means that the IOMMU's PMRs
		 * won't be disabled via the call to init_dmars(). So disable
		 * it explicitly here. The PMRs were setup by tboot prior to
		 * calling SENTER, but the kernel is expected to reset/tear
		 * down the PMRs.
		 */
		if (intel_iommu_tboot_noforce) {
			for_each_iommu(iommu, drhd)
				iommu_disable_protect_mem_regions(iommu);
		}

		/*
		 * Make sure the IOMMUs are switched off, even when we
		 * boot into a kexec kernel and the previous kernel left
		 * them enabled
		 */
		intel_disable_iommus();
		goto out_free_dmar;
	}

	if (list_empty(&dmar_rmrr_units))
		pr_info("No RMRR found\n");

	if (list_empty(&dmar_atsr_units))
		pr_info("No ATSR found\n");

	if (dmar_init_reserved_ranges()) {
		if (force_on)
			panic("tboot: Failed to reserve iommu ranges\n");
		goto out_free_reserved_range;
	}

	init_no_remapping_devices();

	ret = init_dmars();
	if (ret) {
		if (force_on)
			panic("tboot: Failed to initialize DMARs\n");
		pr_err("Initialization failed\n");
		goto out_free_reserved_range;
	}
	up_write(&dmar_global_lock);
	pr_info("Intel(R) Virtualization Technology for Directed I/O\n");

#if defined(CONFIG_X86) && defined(CONFIG_SWIOTLB)
	swiotlb = 0;
#endif
	dma_ops = &intel_dma_ops;

	init_iommu_pm_ops();

	for_each_active_iommu(iommu, drhd) {
		iommu_device_sysfs_add(&iommu->iommu, NULL,
				       intel_iommu_groups,
				       "%s", iommu->name);
		iommu_device_set_ops(&iommu->iommu, &intel_iommu_ops);
		iommu_device_register(&iommu->iommu);
	}

	bus_set_iommu(&pci_bus_type, &intel_iommu_ops);
	bus_register_notifier(&pci_bus_type, &device_nb);
	if (si_domain && !hw_pass_through)
		register_memory_notifier(&intel_iommu_memory_nb);
	cpuhp_setup_state(CPUHP_IOMMU_INTEL_DEAD, "iommu/intel:dead", NULL,
			  intel_iommu_cpu_dead);
	intel_iommu_enabled = 1;
	intel_iommu_debugfs_init();

	return 0;

out_free_reserved_range:
	put_iova_domain(&reserved_iova_list);
out_free_dmar:
	intel_iommu_free_dmars();
	up_write(&dmar_global_lock);
	iommu_exit_mempool();
	return ret;
}