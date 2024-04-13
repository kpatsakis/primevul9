static void intel_iommu_free_atsr(struct dmar_atsr_unit *atsru)
{
	dmar_free_dev_scope(&atsru->devices, &atsru->devices_cnt);
	kfree(atsru);
}