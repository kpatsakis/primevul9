int dmar_check_one_atsr(struct acpi_dmar_header *hdr, void *arg)
{
	int i;
	struct device *dev;
	struct acpi_dmar_atsr *atsr;
	struct dmar_atsr_unit *atsru;

	atsr = container_of(hdr, struct acpi_dmar_atsr, header);
	atsru = dmar_find_atsr(atsr);
	if (!atsru)
		return 0;

	if (!atsru->include_all && atsru->devices && atsru->devices_cnt) {
		for_each_active_dev_scope(atsru->devices, atsru->devices_cnt,
					  i, dev)
			return -EBUSY;
	}

	return 0;
}