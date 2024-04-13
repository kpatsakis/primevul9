static int domain_prepare_identity_map(struct device *dev,
				       struct dmar_domain *domain,
				       unsigned long long start,
				       unsigned long long end)
{
	/* For _hardware_ passthrough, don't bother. But for software
	   passthrough, we do it anyway -- it may indicate a memory
	   range which is reserved in E820, so which didn't get set
	   up to start with in si_domain */
	if (domain == si_domain && hw_pass_through) {
		pr_warn("Ignoring identity map for HW passthrough device %s [0x%Lx - 0x%Lx]\n",
			dev_name(dev), start, end);
		return 0;
	}

	pr_info("Setting identity map for device %s [0x%Lx - 0x%Lx]\n",
		dev_name(dev), start, end);

	if (end < start) {
		WARN(1, "Your BIOS is broken; RMRR ends before it starts!\n"
			"BIOS vendor: %s; Ver: %s; Product Version: %s\n",
			dmi_get_system_info(DMI_BIOS_VENDOR),
			dmi_get_system_info(DMI_BIOS_VERSION),
		     dmi_get_system_info(DMI_PRODUCT_VERSION));
		return -EIO;
	}

	if (end >> agaw_to_width(domain->agaw)) {
		WARN(1, "Your BIOS is broken; RMRR exceeds permitted address width (%d bits)\n"
		     "BIOS vendor: %s; Ver: %s; Product Version: %s\n",
		     agaw_to_width(domain->agaw),
		     dmi_get_system_info(DMI_BIOS_VENDOR),
		     dmi_get_system_info(DMI_BIOS_VERSION),
		     dmi_get_system_info(DMI_PRODUCT_VERSION));
		return -EIO;
	}

	return iommu_domain_identity_map(domain, start, end);
}