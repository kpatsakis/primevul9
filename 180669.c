static void quirk_iommu_g4x_gfx(struct pci_dev *dev)
{
	/* G4x/GM45 integrated gfx dmar support is totally busted. */
	pr_info("Disabling IOMMU for graphics on this chipset\n");
	dmar_map_gfx = 0;
}