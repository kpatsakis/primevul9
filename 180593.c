static void quirk_ioat_snb_local_iommu(struct pci_dev *pdev)
{
	struct dmar_drhd_unit *drhd;
	u32 vtbar;
	int rc;

	/* We know that this device on this chipset has its own IOMMU.
	 * If we find it under a different IOMMU, then the BIOS is lying
	 * to us. Hope that the IOMMU for this device is actually
	 * disabled, and it needs no translation...
	 */
	rc = pci_bus_read_config_dword(pdev->bus, PCI_DEVFN(0, 0), 0xb0, &vtbar);
	if (rc) {
		/* "can't" happen */
		dev_info(&pdev->dev, "failed to run vt-d quirk\n");
		return;
	}
	vtbar &= 0xffff0000;

	/* we know that the this iommu should be at offset 0xa000 from vtbar */
	drhd = dmar_find_matched_drhd_unit(pdev);
	if (WARN_TAINT_ONCE(!drhd || drhd->reg_base_addr - vtbar != 0xa000,
			    TAINT_FIRMWARE_WORKAROUND,
			    "BIOS assigned incorrect VT-d unit for Intel(R) QuickData Technology device\n"))
		pdev->dev.archdata.iommu = DUMMY_DEVICE_DOMAIN_INFO;
}