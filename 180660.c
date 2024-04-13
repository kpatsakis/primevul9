int dmar_find_matched_atsr_unit(struct pci_dev *dev)
{
	int i, ret = 1;
	struct pci_bus *bus;
	struct pci_dev *bridge = NULL;
	struct device *tmp;
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	dev = pci_physfn(dev);
	for (bus = dev->bus; bus; bus = bus->parent) {
		bridge = bus->self;
		/* If it's an integrated device, allow ATS */
		if (!bridge)
			return 1;
		/* Connected via non-PCIe: no ATS */
		if (!pci_is_pcie(bridge) ||
		    pci_pcie_type(bridge) == PCI_EXP_TYPE_PCI_BRIDGE)
			return 0;
		/* If we found the root port, look it up in the ATSR */
		if (pci_pcie_type(bridge) == PCI_EXP_TYPE_ROOT_PORT)
			break;
	}

	rcu_read_lock();
	list_for_each_entry_rcu(atsru, &dmar_atsr_units, list) {
		atsr = container_of(atsru->hdr, struct acpi_dmar_atsr, header);
		if (atsr->segment != pci_domain_nr(dev->bus))
			continue;

		for_each_dev_scope(atsru->devices, atsru->devices_cnt, i, tmp)
			if (tmp == &bridge->dev)
				goto out;

		if (atsru->include_all)
			goto out;
	}
	ret = 0;
out:
	rcu_read_unlock();

	return ret;
}