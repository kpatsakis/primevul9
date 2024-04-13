static int __init hsm_init(void)
{
	int ret;

	if (x86_hyper_type != X86_HYPER_ACRN)
		return -ENODEV;

	if (!(cpuid_eax(ACRN_CPUID_FEATURES) & ACRN_FEATURE_PRIVILEGED_VM))
		return -EPERM;

	ret = misc_register(&acrn_dev);
	if (ret) {
		pr_err("Create misc dev failed!\n");
		return ret;
	}

	ret = acrn_ioreq_intr_setup();
	if (ret) {
		pr_err("Setup I/O request handler failed!\n");
		misc_deregister(&acrn_dev);
		return ret;
	}
	return 0;
}