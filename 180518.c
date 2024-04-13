static int __init intel_iommu_setup(char *str)
{
	if (!str)
		return -EINVAL;
	while (*str) {
		if (!strncmp(str, "on", 2)) {
			dmar_disabled = 0;
			pr_info("IOMMU enabled\n");
		} else if (!strncmp(str, "off", 3)) {
			dmar_disabled = 1;
			no_platform_optin = 1;
			pr_info("IOMMU disabled\n");
		} else if (!strncmp(str, "igfx_off", 8)) {
			dmar_map_gfx = 0;
			pr_info("Disable GFX device mapping\n");
		} else if (!strncmp(str, "forcedac", 8)) {
			pr_info("Forcing DAC for PCI devices\n");
			dmar_forcedac = 1;
		} else if (!strncmp(str, "strict", 6)) {
			pr_info("Disable batched IOTLB flush\n");
			intel_iommu_strict = 1;
		} else if (!strncmp(str, "sp_off", 6)) {
			pr_info("Disable supported super page\n");
			intel_iommu_superpage = 0;
		} else if (!strncmp(str, "ecs_off", 7)) {
			printk(KERN_INFO
				"Intel-IOMMU: disable extended context table support\n");
			intel_iommu_ecs = 0;
		} else if (!strncmp(str, "pasid28", 7)) {
			printk(KERN_INFO
				"Intel-IOMMU: enable pre-production PASID support\n");
			intel_iommu_pasid28 = 1;
			iommu_identity_mapping |= IDENTMAP_GFX;
		} else if (!strncmp(str, "tboot_noforce", 13)) {
			printk(KERN_INFO
				"Intel-IOMMU: not forcing on after tboot. This could expose security risk for tboot\n");
			intel_iommu_tboot_noforce = 1;
		}

		str += strcspn(str, ",");
		while (*str == ',')
			str++;
	}
	return 0;
}