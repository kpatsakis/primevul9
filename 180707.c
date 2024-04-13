int __init dmar_parse_one_rmrr(struct acpi_dmar_header *header, void *arg)
{
	struct acpi_dmar_reserved_memory *rmrr;
	int prot = DMA_PTE_READ|DMA_PTE_WRITE;
	struct dmar_rmrr_unit *rmrru;
	size_t length;

	rmrru = kzalloc(sizeof(*rmrru), GFP_KERNEL);
	if (!rmrru)
		goto out;

	rmrru->hdr = header;
	rmrr = (struct acpi_dmar_reserved_memory *)header;
	rmrru->base_address = rmrr->base_address;
	rmrru->end_address = rmrr->end_address;

	length = rmrr->end_address - rmrr->base_address + 1;
	rmrru->resv = iommu_alloc_resv_region(rmrr->base_address, length, prot,
					      IOMMU_RESV_DIRECT);
	if (!rmrru->resv)
		goto free_rmrru;

	rmrru->devices = dmar_alloc_dev_scope((void *)(rmrr + 1),
				((void *)rmrr) + rmrr->header.length,
				&rmrru->devices_cnt);
	if (rmrru->devices_cnt && rmrru->devices == NULL)
		goto free_all;

	list_add(&rmrru->list, &dmar_rmrr_units);

	return 0;
free_all:
	kfree(rmrru->resv);
free_rmrru:
	kfree(rmrru);
out:
	return -ENOMEM;
}