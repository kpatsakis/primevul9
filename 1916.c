static bool svm_get_nested_state_pages(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	if (WARN_ON(!is_guest_mode(vcpu)))
		return true;

	if (!vcpu->arch.pdptrs_from_userspace &&
	    !nested_npt_enabled(svm) && is_pae_paging(vcpu))
		/*
		 * Reload the guest's PDPTRs since after a migration
		 * the guest CR3 might be restored prior to setting the nested
		 * state which can lead to a load of wrong PDPTRs.
		 */
		if (CC(!load_pdptrs(vcpu, vcpu->arch.walk_mmu, vcpu->arch.cr3)))
			return false;

	if (!nested_svm_vmrun_msrpm(svm)) {
		vcpu->run->exit_reason = KVM_EXIT_INTERNAL_ERROR;
		vcpu->run->internal.suberror =
			KVM_INTERNAL_ERROR_EMULATION;
		vcpu->run->internal.ndata = 0;
		return false;
	}

	return true;
}