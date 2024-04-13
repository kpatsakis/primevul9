static void svm_load_mmu_pgd(struct kvm_vcpu *vcpu, unsigned long root)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	bool update_guest_cr3 = true;
	unsigned long cr3;

	cr3 = __sme_set(root);
	if (npt_enabled) {
		svm->vmcb->control.nested_cr3 = cr3;
		mark_dirty(svm->vmcb, VMCB_NPT);

		/* Loading L2's CR3 is handled by enter_svm_guest_mode.  */
		if (is_guest_mode(vcpu))
			update_guest_cr3 = false;
		else if (test_bit(VCPU_EXREG_CR3, (ulong *)&vcpu->arch.regs_avail))
			cr3 = vcpu->arch.cr3;
		else /* CR3 is already up-to-date.  */
			update_guest_cr3 = false;
	}

	if (update_guest_cr3) {
		svm->vmcb->save.cr3 = cr3;
		mark_dirty(svm->vmcb, VMCB_CR);
	}
}