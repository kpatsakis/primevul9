static bool nested_vmcb_valid_sregs(struct kvm_vcpu *vcpu,
				    struct vmcb_save_area *save)
{
	/*
	 * FIXME: these should be done after copying the fields,
	 * to avoid TOC/TOU races.  For these save area checks
	 * the possible damage is limited since kvm_set_cr0 and
	 * kvm_set_cr4 handle failure; EFER_SVME is an exception
	 * so it is force-set later in nested_prepare_vmcb_save.
	 */
	if (CC(!(save->efer & EFER_SVME)))
		return false;

	if (CC((save->cr0 & X86_CR0_CD) == 0 && (save->cr0 & X86_CR0_NW)) ||
	    CC(save->cr0 & ~0xffffffffULL))
		return false;

	if (CC(!kvm_dr6_valid(save->dr6)) || CC(!kvm_dr7_valid(save->dr7)))
		return false;

	if (!nested_vmcb_check_cr3_cr4(vcpu, save))
		return false;

	if (CC(!kvm_valid_efer(vcpu, save->efer)))
		return false;

	return true;
}