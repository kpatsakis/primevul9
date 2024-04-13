static void kvm_pv_guest_cpu_reboot(void *unused)
{
	/*
	 * We disable PV EOI before we load a new kernel by kexec,
	 * since MSR_KVM_PV_EOI_EN stores a pointer into old kernel's memory.
	 * New kernel can re-enable when it boots.
	 */
	if (kvm_para_has_feature(KVM_FEATURE_PV_EOI))
		wrmsrl(MSR_KVM_PV_EOI_EN, 0);
	kvm_pv_disable_apf();
	kvm_disable_steal_time();
}