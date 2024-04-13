static int dwc3_pci_dsm(struct dwc3_pci *dwc, int param)
{
	union acpi_object *obj;
	union acpi_object tmp;
	union acpi_object argv4 = ACPI_INIT_DSM_ARGV4(1, &tmp);

	if (!dwc->has_dsm_for_pm)
		return 0;

	tmp.type = ACPI_TYPE_INTEGER;
	tmp.integer.value = param;

	obj = acpi_evaluate_dsm(ACPI_HANDLE(&dwc->pci->dev), &dwc->guid,
			1, PCI_INTEL_BXT_FUNC_PMU_PWR, &argv4);
	if (!obj) {
		dev_err(&dwc->pci->dev, "failed to evaluate _DSM\n");
		return -EIO;
	}

	ACPI_FREE(obj);

	return 0;
}