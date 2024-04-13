static int dmar_init_reserved_ranges(void)
{
	struct pci_dev *pdev = NULL;
	struct iova *iova;
	int i;

	init_iova_domain(&reserved_iova_list, VTD_PAGE_SIZE, IOVA_START_PFN);

	lockdep_set_class(&reserved_iova_list.iova_rbtree_lock,
		&reserved_rbtree_key);

	/* IOAPIC ranges shouldn't be accessed by DMA */
	iova = reserve_iova(&reserved_iova_list, IOVA_PFN(IOAPIC_RANGE_START),
		IOVA_PFN(IOAPIC_RANGE_END));
	if (!iova) {
		pr_err("Reserve IOAPIC range failed\n");
		return -ENODEV;
	}

	/* Reserve all PCI MMIO to avoid peer-to-peer access */
	for_each_pci_dev(pdev) {
		struct resource *r;

		for (i = 0; i < PCI_NUM_RESOURCES; i++) {
			r = &pdev->resource[i];
			if (!r->flags || !(r->flags & IORESOURCE_MEM))
				continue;
			iova = reserve_iova(&reserved_iova_list,
					    IOVA_PFN(r->start),
					    IOVA_PFN(r->end));
			if (!iova) {
				pci_err(pdev, "Reserve iova for %pR failed\n", r);
				return -ENODEV;
			}
		}
	}
	return 0;
}