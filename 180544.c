static struct dmar_atsr_unit *dmar_find_atsr(struct acpi_dmar_atsr *atsr)
{
	struct dmar_atsr_unit *atsru;
	struct acpi_dmar_atsr *tmp;

	list_for_each_entry_rcu(atsru, &dmar_atsr_units, list) {
		tmp = (struct acpi_dmar_atsr *)atsru->hdr;
		if (atsr->segment != tmp->segment)
			continue;
		if (atsr->header.length != tmp->header.length)
			continue;
		if (memcmp(atsr, tmp, atsr->header.length) == 0)
			return atsru;
	}

	return NULL;
}