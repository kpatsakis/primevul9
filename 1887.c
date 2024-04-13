static bool nested_vmcb_check_cr3_cr4(struct kvm_vcpu *vcpu,
				      struct vmcb_save_area *save)
{
	/*
	 * These checks are also performed by KVM_SET_SREGS,
	 * except that EFER.LMA is not checked by SVM against
	 * CR0.PG && EFER.LME.
	 */
	if ((save->efer & EFER_LME) && (save->cr0 & X86_CR0_PG)) {
		if (CC(!(save->cr4 & X86_CR4_PAE)) ||
		    CC(!(save->cr0 & X86_CR0_PE)) ||
		    CC(kvm_vcpu_is_illegal_gpa(vcpu, save->cr3)))
			return false;
	}

	if (CC(!kvm_is_valid_cr4(vcpu, save->cr4)))
		return false;

	return true;
}