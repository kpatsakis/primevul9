static struct dmar_domain *dmar_insert_one_dev_info(struct intel_iommu *iommu,
						    int bus, int devfn,
						    struct device *dev,
						    struct dmar_domain *domain)
{
	struct dmar_domain *found = NULL;
	struct device_domain_info *info;
	unsigned long flags;
	int ret;

	info = alloc_devinfo_mem();
	if (!info)
		return NULL;

	info->bus = bus;
	info->devfn = devfn;
	info->ats_supported = info->pasid_supported = info->pri_supported = 0;
	info->ats_enabled = info->pasid_enabled = info->pri_enabled = 0;
	info->ats_qdep = 0;
	info->dev = dev;
	info->domain = domain;
	info->iommu = iommu;
	info->pasid_table = NULL;

	if (dev && dev_is_pci(dev)) {
		struct pci_dev *pdev = to_pci_dev(info->dev);

		if (!pci_ats_disabled() &&
		    ecap_dev_iotlb_support(iommu->ecap) &&
		    pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_ATS) &&
		    dmar_find_matched_atsr_unit(pdev))
			info->ats_supported = 1;

		if (ecs_enabled(iommu)) {
			if (pasid_enabled(iommu)) {
				int features = pci_pasid_features(pdev);
				if (features >= 0)
					info->pasid_supported = features | 1;
			}

			if (info->ats_supported && ecap_prs(iommu->ecap) &&
			    pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_PRI))
				info->pri_supported = 1;
		}
	}

	spin_lock_irqsave(&device_domain_lock, flags);
	if (dev)
		found = find_domain(dev);

	if (!found) {
		struct device_domain_info *info2;
		info2 = dmar_search_domain_by_dev_info(iommu->segment, bus, devfn);
		if (info2) {
			found      = info2->domain;
			info2->dev = dev;
		}
	}

	if (found) {
		spin_unlock_irqrestore(&device_domain_lock, flags);
		free_devinfo_mem(info);
		/* Caller must free the original domain */
		return found;
	}

	spin_lock(&iommu->lock);
	ret = domain_attach_iommu(domain, iommu);
	spin_unlock(&iommu->lock);

	if (ret) {
		spin_unlock_irqrestore(&device_domain_lock, flags);
		free_devinfo_mem(info);
		return NULL;
	}

	list_add(&info->link, &domain->devices);
	list_add(&info->global, &device_domain_list);
	if (dev)
		dev->archdata.iommu = info;

	if (dev && dev_is_pci(dev) && info->pasid_supported) {
		ret = intel_pasid_alloc_table(dev);
		if (ret) {
			pr_warn("No pasid table for %s, pasid disabled\n",
				dev_name(dev));
			info->pasid_supported = 0;
		}
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);

	if (dev && domain_context_mapping(domain, dev)) {
		pr_err("Domain context map for %s failed\n", dev_name(dev));
		dmar_remove_one_dev_info(domain, dev);
		return NULL;
	}

	return domain;
}