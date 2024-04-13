emulate_sysenter(struct x86_emulate_ctxt *ctxt)
{
	struct decode_cache *c = &ctxt->decode;
	struct kvm_segment cs, ss;
	u64 msr_data;

	/* inject #UD if LOCK prefix is used */
	if (c->lock_prefix)
		return -1;

	/* inject #GP if in real mode or paging is disabled */
	if (ctxt->mode == X86EMUL_MODE_REAL ||
		!(ctxt->vcpu->arch.cr0 & X86_CR0_PE)) {
		kvm_inject_gp(ctxt->vcpu, 0);
		return -1;
	}

	/* XXX sysenter/sysexit have not been tested in 64bit mode.
	* Therefore, we inject an #UD.
	*/
	if (ctxt->mode == X86EMUL_MODE_PROT64)
		return -1;

	setup_syscalls_segments(ctxt, &cs, &ss);

	kvm_x86_ops->get_msr(ctxt->vcpu, MSR_IA32_SYSENTER_CS, &msr_data);
	switch (ctxt->mode) {
	case X86EMUL_MODE_PROT32:
		if ((msr_data & 0xfffc) == 0x0) {
			kvm_inject_gp(ctxt->vcpu, 0);
			return -1;
		}
		break;
	case X86EMUL_MODE_PROT64:
		if (msr_data == 0x0) {
			kvm_inject_gp(ctxt->vcpu, 0);
			return -1;
		}
		break;
	}

	ctxt->eflags &= ~(EFLG_VM | EFLG_IF | EFLG_RF);
	cs.selector = (u16)msr_data;
	cs.selector &= ~SELECTOR_RPL_MASK;
	ss.selector = cs.selector + 8;
	ss.selector &= ~SELECTOR_RPL_MASK;
	if (ctxt->mode == X86EMUL_MODE_PROT64
		|| is_long_mode(ctxt->vcpu)) {
		cs.db = 0;
		cs.l = 1;
	}

	kvm_x86_ops->set_segment(ctxt->vcpu, &cs, VCPU_SREG_CS);
	kvm_x86_ops->set_segment(ctxt->vcpu, &ss, VCPU_SREG_SS);

	kvm_x86_ops->get_msr(ctxt->vcpu, MSR_IA32_SYSENTER_EIP, &msr_data);
	c->eip = msr_data;

	kvm_x86_ops->get_msr(ctxt->vcpu, MSR_IA32_SYSENTER_ESP, &msr_data);
	c->regs[VCPU_REGS_RSP] = msr_data;

	return 0;
}