int kvm_handle_page_fault(struct kvm_vcpu *vcpu, u64 error_code,
				u64 fault_address, char *insn, int insn_len)
{
	int r = 1;
	u32 flags = vcpu->arch.apf.host_apf_flags;

#ifndef CONFIG_X86_64
	/* A 64-bit CR2 should be impossible on 32-bit KVM. */
	if (WARN_ON_ONCE(fault_address >> 32))
		return -EFAULT;
#endif

	vcpu->arch.l1tf_flush_l1d = true;
	if (!flags) {
		trace_kvm_page_fault(fault_address, error_code);

		if (kvm_event_needs_reinjection(vcpu))
			kvm_mmu_unprotect_page_virt(vcpu, fault_address);
		r = kvm_mmu_page_fault(vcpu, fault_address, error_code, insn,
				insn_len);
	} else if (flags & KVM_PV_REASON_PAGE_NOT_PRESENT) {
		vcpu->arch.apf.host_apf_flags = 0;
		local_irq_disable();
		kvm_async_pf_task_wait_schedule(fault_address);
		local_irq_enable();
	} else {
		WARN_ONCE(1, "Unexpected host async PF flags: %x\n", flags);
	}

	return r;
}