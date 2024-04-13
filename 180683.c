static int __init platform_optin_force_iommu(void)
{
	struct pci_dev *pdev = NULL;
	bool has_untrusted_dev = false;

	if (!dmar_platform_optin() || no_platform_optin)
		return 0;

	for_each_pci_dev(pdev) {
		if (pdev->untrusted) {
			has_untrusted_dev = true;
			break;
		}
	}

	if (!has_untrusted_dev)
		return 0;

	if (no_iommu || dmar_disabled)
		pr_info("Intel-IOMMU force enabled due to platform opt in\n");

	/*
	 * If Intel-IOMMU is disabled by default, we will apply identity
	 * map for all devices except those marked as being untrusted.
	 */
	if (dmar_disabled)
		iommu_identity_mapping |= IDENTMAP_ALL;

	dmar_disabled = 0;
#if defined(CONFIG_X86) && defined(CONFIG_SWIOTLB)
	swiotlb = 0;
#endif
	no_iommu = 0;

	return 1;
}