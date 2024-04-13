static void __init check_tylersburg_isoch(void)
{
	struct pci_dev *pdev;
	uint32_t vtisochctrl;

	/* If there's no Azalia in the system anyway, forget it. */
	pdev = pci_get_device(PCI_VENDOR_ID_INTEL, 0x3a3e, NULL);
	if (!pdev)
		return;
	pci_dev_put(pdev);

	/* System Management Registers. Might be hidden, in which case
	   we can't do the sanity check. But that's OK, because the
	   known-broken BIOSes _don't_ actually hide it, so far. */
	pdev = pci_get_device(PCI_VENDOR_ID_INTEL, 0x342e, NULL);
	if (!pdev)
		return;

	if (pci_read_config_dword(pdev, 0x188, &vtisochctrl)) {
		pci_dev_put(pdev);
		return;
	}

	pci_dev_put(pdev);

	/* If Azalia DMA is routed to the non-isoch DMAR unit, fine. */
	if (vtisochctrl & 1)
		return;

	/* Drop all bits other than the number of TLB entries */
	vtisochctrl &= 0x1c;

	/* If we have the recommended number of TLB entries (16), fine. */
	if (vtisochctrl == 0x10)
		return;

	/* Zero TLB entries? You get to ride the short bus to school. */
	if (!vtisochctrl) {
		WARN(1, "Your BIOS is broken; DMA routed to ISOCH DMAR unit but no TLB space.\n"
		     "BIOS vendor: %s; Ver: %s; Product Version: %s\n",
		     dmi_get_system_info(DMI_BIOS_VENDOR),
		     dmi_get_system_info(DMI_BIOS_VERSION),
		     dmi_get_system_info(DMI_PRODUCT_VERSION));
		iommu_identity_mapping |= IDENTMAP_AZALIA;
		return;
	}

	pr_warn("Recommended TLB entries for ISOCH unit is 16; your BIOS set %d\n",
	       vtisochctrl);
}