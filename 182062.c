static void __init paravirt_ops_setup(void)
{
	pv_info.name = "KVM";

	/*
	 * KVM isn't paravirt in the sense of paravirt_enabled.  A KVM
	 * guest kernel works like a bare metal kernel with additional
	 * features, and paravirt_enabled is about features that are
	 * missing.
	 */
	pv_info.paravirt_enabled = 0;

	if (kvm_para_has_feature(KVM_FEATURE_NOP_IO_DELAY))
		pv_cpu_ops.io_delay = kvm_io_delay;

#ifdef CONFIG_X86_IO_APIC
	no_timer_check = 1;
#endif
}