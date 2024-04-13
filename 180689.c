int dmar_release_one_atsr(struct acpi_dmar_header *hdr, void *arg)
{
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	atsr = container_of(hdr, struct acpi_dmar_atsr, header);
	atsru = dmar_find_atsr(atsr);
	if (atsru) {
		list_del_rcu(&atsru->list);
		synchronize_rcu();
		intel_iommu_free_atsr(atsru);
	}

	return 0;
}