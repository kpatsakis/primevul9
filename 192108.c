static void __mcheck_cpu_clear_vendor(struct cpuinfo_x86 *c)
{
	switch (c->x86_vendor) {
	case X86_VENDOR_INTEL:
		mce_intel_feature_clear(c);
		break;
	default:
		break;
	}
}