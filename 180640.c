static int domain_context_mapping_cb(struct pci_dev *pdev,
				     u16 alias, void *opaque)
{
	struct domain_context_mapping_data *data = opaque;

	return domain_context_mapping_one(data->domain, data->iommu,
					  PCI_BUS_NUM(alias), alias & 0xff);
}