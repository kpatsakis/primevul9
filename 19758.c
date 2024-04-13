static void sev_es_sync_from_ghcb(struct vcpu_svm *svm)
{
	struct vmcb_control_area *control = &svm->vmcb->control;
	struct kvm_vcpu *vcpu = &svm->vcpu;
	struct ghcb *ghcb = svm->ghcb;
	u64 exit_code;

	/*
	 * The GHCB protocol so far allows for the following data
	 * to be supplied:
	 *   GPRs RAX, RBX, RCX, RDX
	 *   XCR0
	 *   CPL
	 *
	 * VMMCALL allows the guest to provide extra registers. KVM also
	 * expects RSI for hypercalls, so include that, too.
	 *
	 * Copy their values to the appropriate location if supplied.
	 */
	memset(vcpu->arch.regs, 0, sizeof(vcpu->arch.regs));

	vcpu->arch.regs[VCPU_REGS_RAX] = ghcb_get_rax_if_valid(ghcb);
	vcpu->arch.regs[VCPU_REGS_RBX] = ghcb_get_rbx_if_valid(ghcb);
	vcpu->arch.regs[VCPU_REGS_RCX] = ghcb_get_rcx_if_valid(ghcb);
	vcpu->arch.regs[VCPU_REGS_RDX] = ghcb_get_rdx_if_valid(ghcb);
	vcpu->arch.regs[VCPU_REGS_RSI] = ghcb_get_rsi_if_valid(ghcb);

	svm->vmcb->save.cpl = ghcb_get_cpl_if_valid(ghcb);

	if (ghcb_xcr0_is_valid(ghcb)) {
		vcpu->arch.xcr0 = ghcb_get_xcr0(ghcb);
		kvm_update_cpuid_runtime(vcpu);
	}

	/* Copy the GHCB exit information into the VMCB fields */
	exit_code = ghcb_get_sw_exit_code(ghcb);
	control->exit_code = lower_32_bits(exit_code);
	control->exit_code_hi = upper_32_bits(exit_code);
	control->exit_info_1 = ghcb_get_sw_exit_info_1(ghcb);
	control->exit_info_2 = ghcb_get_sw_exit_info_2(ghcb);

	/* Clear the valid entries fields */
	memset(ghcb->save.valid_bitmap, 0, sizeof(ghcb->save.valid_bitmap));
}