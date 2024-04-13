static void intel_iommu_get_resv_regions(struct device *device,
					 struct list_head *head)
{
	struct iommu_resv_region *reg;
	struct dmar_rmrr_unit *rmrr;
	struct device *i_dev;
	int i;

	rcu_read_lock();
	for_each_rmrr_units(rmrr) {
		for_each_active_dev_scope(rmrr->devices, rmrr->devices_cnt,
					  i, i_dev) {
			if (i_dev != device)
				continue;

			list_add_tail(&rmrr->resv->list, head);
		}
	}
	rcu_read_unlock();

	reg = iommu_alloc_resv_region(IOAPIC_RANGE_START,
				      IOAPIC_RANGE_END - IOAPIC_RANGE_START + 1,
				      0, IOMMU_RESV_MSI);
	if (!reg)
		return;
	list_add_tail(&reg->list, head);
}