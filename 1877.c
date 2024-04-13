static void nested_svm_init_mmu_context(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	WARN_ON(mmu_is_nested(vcpu));

	vcpu->arch.mmu = &vcpu->arch.guest_mmu;

	/*
	 * The NPT format depends on L1's CR4 and EFER, which is in vmcb01.  Note,
	 * when called via KVM_SET_NESTED_STATE, that state may _not_ match current
	 * vCPU state.  CR0.WP is explicitly ignored, while CR0.PG is required.
	 */
	kvm_init_shadow_npt_mmu(vcpu, X86_CR0_PG, svm->vmcb01.ptr->save.cr4,
				svm->vmcb01.ptr->save.efer,
				svm->nested.ctl.nested_cr3);
	vcpu->arch.mmu->get_guest_pgd     = nested_svm_get_tdp_cr3;
	vcpu->arch.mmu->get_pdptr         = nested_svm_get_tdp_pdptr;
	vcpu->arch.mmu->inject_page_fault = nested_svm_inject_npf_exit;
	vcpu->arch.walk_mmu              = &vcpu->arch.nested_mmu;
}