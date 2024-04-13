emulate_sysexit(struct x86_emulate_ctxt *ctxt)
{
	struct decode_cache *c = &ctxt->decode;
	struct kvm_segment cs, ss;
	u64 msr_data;
	int usermode;

	/* inject #UD if LOCK prefix is used */
	if (c->lock_prefix)
		return -1;

	/* inject #GP if in real mode or paging is disabled */
	if (ctxt->mode == X86EMUL_MODE_REAL
		|| !(ctxt->vcpu->arch.cr0 & X86_CR0_PE)) {
		kvm_inject_gp(ctxt->vcpu, 0);
		return -1;
	}

	/* sysexit must be called from CPL 0 */
	if (kvm_x86_ops->get_cpl(ctxt->vcpu) != 0) {
		kvm_inject_gp(ctxt->vcpu, 0);
		return -1;
	}

	setup_syscalls_segments(ctxt, &cs, &ss);

	if ((c->rex_prefix & 0x8) != 0x0)
		usermode = X86EMUL_MODE_PROT64;
	else
		usermode = X86EMUL_MODE_PROT32;

	cs.dpl = 3;
	ss.dpl = 3;
	kvm_x86_ops->get_msr(ctxt->vcpu, MSR_IA32_SYSENTER_CS, &msr_data);
	switch (usermode) {
	case X86EMUL_MODE_PROT32:
		cs.selector = (u16)(msr_data + 16);
		if ((msr_data & 0xfffc) == 0x0) {
			kvm_inject_gp(ctxt->vcpu, 0);
			return -1;
		}
		ss.selector = (u16)(msr_data + 24);
		break;
	case X86EMUL_MODE_PROT64:
		cs.selector = (u16)(msr_data + 32);
		if (msr_data == 0x0) {
			kvm_inject_gp(ctxt->vcpu, 0);
			return -1;
		}
		ss.selector = cs.selector + 8;
		cs.db = 0;
		cs.l = 1;
		break;
	}
	cs.selector |= SELECTOR_RPL_MASK;
	ss.selector |= SELECTOR_RPL_MASK;

	kvm_x86_ops->set_segment(ctxt->vcpu, &cs, VCPU_SREG_CS);
	kvm_x86_ops->set_segment(ctxt->vcpu, &ss, VCPU_SREG_SS);

	c->eip = ctxt->vcpu->arch.regs[VCPU_REGS_RDX];
	c->regs[VCPU_REGS_RSP] = ctxt->vcpu->arch.regs[VCPU_REGS_RCX];

	return 0;
}