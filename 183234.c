static int intel_iommu_attach_device(struct iommu_domain *domain,
				     struct device *dev)
{
	struct dmar_domain *dmar_domain = to_dmar_domain(domain);
	struct intel_iommu *iommu;
	int addr_width;
	u8 bus, devfn;

	if (device_is_rmrr_locked(dev)) {
		dev_warn(dev, "Device is ineligible for IOMMU domain attach due to platform RMRR requirement.  Contact your platform vendor.\n");
		return -EPERM;
	}

	/* normally dev is not mapped */
	if (unlikely(domain_context_mapped(dev))) {
		struct dmar_domain *old_domain;

		old_domain = find_domain(dev);
		if (old_domain) {
			rcu_read_lock();
			dmar_remove_one_dev_info(dev);
			rcu_read_unlock();

			if (!domain_type_is_vm_or_si(old_domain) &&
			     list_empty(&old_domain->devices))
				domain_exit(old_domain);
		}
	}

	iommu = device_to_iommu(dev, &bus, &devfn);
	if (!iommu)
		return -ENODEV;

	/* check if this iommu agaw is sufficient for max mapped address */
	addr_width = agaw_to_width(iommu->agaw);
	if (addr_width > cap_mgaw(iommu->cap))
		addr_width = cap_mgaw(iommu->cap);

	if (dmar_domain->max_addr > (1LL << addr_width)) {
		dev_err(dev, "%s: iommu width (%d) is not "
		        "sufficient for the mapped address (%llx)\n",
		        __func__, addr_width, dmar_domain->max_addr);
		return -EFAULT;
	}
	dmar_domain->gaw = addr_width;

	/*
	 * Knock out extra levels of page tables if necessary
	 */
	while (iommu->agaw < dmar_domain->agaw) {
		struct dma_pte *pte;

		pte = dmar_domain->pgd;
		if (dma_pte_present(pte)) {
			dmar_domain->pgd = (struct dma_pte *)
				phys_to_virt(dma_pte_addr(pte));
			free_pgtable_page(pte);
		}
		dmar_domain->agaw--;
	}

	return domain_add_dev_info(dmar_domain, dev);
}