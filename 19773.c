static void sev_es_sync_to_ghcb(struct vcpu_svm *svm)
{
	struct kvm_vcpu *vcpu = &svm->vcpu;
	struct ghcb *ghcb = svm->ghcb;

	/*
	 * The GHCB protocol so far allows for the following data
	 * to be returned:
	 *   GPRs RAX, RBX, RCX, RDX
	 *
	 * Copy their values, even if they may not have been written during the
	 * VM-Exit.  It's the guest's responsibility to not consume random data.
	 */
	ghcb_set_rax(ghcb, vcpu->arch.regs[VCPU_REGS_RAX]);
	ghcb_set_rbx(ghcb, vcpu->arch.regs[VCPU_REGS_RBX]);
	ghcb_set_rcx(ghcb, vcpu->arch.regs[VCPU_REGS_RCX]);
	ghcb_set_rdx(ghcb, vcpu->arch.regs[VCPU_REGS_RDX]);
}