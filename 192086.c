static void __mcheck_cpu_init_vendor(struct cpuinfo_x86 *c)
{
	switch (c->x86_vendor) {
	case X86_VENDOR_INTEL:
		mce_intel_feature_init(c);
		mce_adjust_timer = cmci_intel_adjust_timer;
		break;

	case X86_VENDOR_AMD: {
		mce_amd_feature_init(c);
		break;
		}

	default:
		break;
	}
}