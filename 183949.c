emulate_syscall(struct x86_emulate_ctxt *ctxt)
{
	struct decode_cache *c = &ctxt->decode;
	struct kvm_segment cs, ss;
	u64 msr_data;

	/* syscall is not available in real mode */
	if (c->lock_prefix || ctxt->mode == X86EMUL_MODE_REAL
		|| !(ctxt->vcpu->arch.cr0 & X86_CR0_PE))
		return -1;

	setup_syscalls_segments(ctxt, &cs, &ss);

	kvm_x86_ops->get_msr(ctxt->vcpu, MSR_STAR, &msr_data);
	msr_data >>= 32;
	cs.selector = (u16)(msr_data & 0xfffc);
	ss.selector = (u16)(msr_data + 8);

	if (is_long_mode(ctxt->vcpu)) {
		cs.db = 0;
		cs.l = 1;
	}
	kvm_x86_ops->set_segment(ctxt->vcpu, &cs, VCPU_SREG_CS);
	kvm_x86_ops->set_segment(ctxt->vcpu, &ss, VCPU_SREG_SS);

	c->regs[VCPU_REGS_RCX] = c->eip;
	if (is_long_mode(ctxt->vcpu)) {
#ifdef CONFIG_X86_64
		c->regs[VCPU_REGS_R11] = ctxt->eflags & ~EFLG_RF;

		kvm_x86_ops->get_msr(ctxt->vcpu,
			ctxt->mode == X86EMUL_MODE_PROT64 ?
			MSR_LSTAR : MSR_CSTAR, &msr_data);
		c->eip = msr_data;

		kvm_x86_ops->get_msr(ctxt->vcpu, MSR_SYSCALL_MASK, &msr_data);
		ctxt->eflags &= ~(msr_data | EFLG_RF);
#endif
	} else {
		/* legacy mode */
		kvm_x86_ops->get_msr(ctxt->vcpu, MSR_STAR, &msr_data);
		c->eip = (u32)msr_data;

		ctxt->eflags &= ~(EFLG_VM | EFLG_IF | EFLG_RF);
	}

	return 0;
}