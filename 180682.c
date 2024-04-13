static void intel_iommu_free_dmars(void)
{
	struct dmar_rmrr_unit *rmrru, *rmrr_n;
	struct dmar_atsr_unit *atsru, *atsr_n;

	list_for_each_entry_safe(rmrru, rmrr_n, &dmar_rmrr_units, list) {
		list_del(&rmrru->list);
		dmar_free_dev_scope(&rmrru->devices, &rmrru->devices_cnt);
		kfree(rmrru->resv);
		kfree(rmrru);
	}

	list_for_each_entry_safe(atsru, atsr_n, &dmar_atsr_units, list) {
		list_del(&atsru->list);
		intel_iommu_free_atsr(atsru);
	}
}