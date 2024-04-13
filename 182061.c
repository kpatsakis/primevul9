void kvm_guest_cpu_init(void)
{
	if (!kvm_para_available())
		return;

	if (kvm_para_has_feature(KVM_FEATURE_ASYNC_PF) && kvmapf) {
		u64 pa = slow_virt_to_phys(this_cpu_ptr(&apf_reason));

#ifdef CONFIG_PREEMPT
		pa |= KVM_ASYNC_PF_SEND_ALWAYS;
#endif
		wrmsrl(MSR_KVM_ASYNC_PF_EN, pa | KVM_ASYNC_PF_ENABLED);
		__this_cpu_write(apf_reason.enabled, 1);
		printk(KERN_INFO"KVM setup async PF for cpu %d\n",
		       smp_processor_id());
	}

	if (kvm_para_has_feature(KVM_FEATURE_PV_EOI)) {
		unsigned long pa;
		/* Size alignment is implied but just to make it explicit. */
		BUILD_BUG_ON(__alignof__(kvm_apic_eoi) < 4);
		__this_cpu_write(kvm_apic_eoi, 0);
		pa = slow_virt_to_phys(this_cpu_ptr(&kvm_apic_eoi))
			| KVM_MSR_ENABLED;
		wrmsrl(MSR_KVM_PV_EOI_EN, pa);
	}

	if (has_steal_clock)
		kvm_register_steal_time();
}