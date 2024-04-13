static struct intel_iommu *device_to_iommu(struct device *dev, u8 *bus, u8 *devfn)
{
	struct dmar_drhd_unit *drhd = NULL;
	struct intel_iommu *iommu;
	struct device *tmp;
	struct pci_dev *ptmp, *pdev = NULL;
	u16 segment = 0;
	int i;

	if (iommu_dummy(dev))
		return NULL;

	if (dev_is_pci(dev)) {
		struct pci_dev *pf_pdev;

		pdev = to_pci_dev(dev);

#ifdef CONFIG_X86
		/* VMD child devices currently cannot be handled individually */
		if (is_vmd(pdev->bus))
			return NULL;
#endif

		/* VFs aren't listed in scope tables; we need to look up
		 * the PF instead to find the IOMMU. */
		pf_pdev = pci_physfn(pdev);
		dev = &pf_pdev->dev;
		segment = pci_domain_nr(pdev->bus);
	} else if (has_acpi_companion(dev))
		dev = &ACPI_COMPANION(dev)->dev;

	rcu_read_lock();
	for_each_active_iommu(iommu, drhd) {
		if (pdev && segment != drhd->segment)
			continue;

		for_each_active_dev_scope(drhd->devices,
					  drhd->devices_cnt, i, tmp) {
			if (tmp == dev) {
				/* For a VF use its original BDF# not that of the PF
				 * which we used for the IOMMU lookup. Strictly speaking
				 * we could do this for all PCI devices; we only need to
				 * get the BDF# from the scope table for ACPI matches. */
				if (pdev && pdev->is_virtfn)
					goto got_pdev;

				*bus = drhd->devices[i].bus;
				*devfn = drhd->devices[i].devfn;
				goto out;
			}

			if (!pdev || !dev_is_pci(tmp))
				continue;

			ptmp = to_pci_dev(tmp);
			if (ptmp->subordinate &&
			    ptmp->subordinate->number <= pdev->bus->number &&
			    ptmp->subordinate->busn_res.end >= pdev->bus->number)
				goto got_pdev;
		}

		if (pdev && drhd->include_all) {
		got_pdev:
			*bus = pdev->bus->number;
			*devfn = pdev->devfn;
			goto out;
		}
	}
	iommu = NULL;
 out:
	rcu_read_unlock();

	return iommu;
}