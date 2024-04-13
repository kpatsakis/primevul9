static void quirk_calpella_no_shadow_gtt(struct pci_dev *dev)
{
	unsigned short ggc;

	if (pci_read_config_word(dev, GGC, &ggc))
		return;

	if (!(ggc & GGC_MEMORY_VT_ENABLED)) {
		pr_info("BIOS has allocated no shadow GTT; disabling IOMMU for graphics\n");
		dmar_map_gfx = 0;
	} else if (dmar_map_gfx) {
		/* we have to ensure the gfx device is idle before we flush */
		pr_info("Disabling batched IOTLB flush on Ironlake\n");
		intel_iommu_strict = 1;
       }
}