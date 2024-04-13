static int nested_svm_load_cr3(struct kvm_vcpu *vcpu, unsigned long cr3,
			       bool nested_npt, bool reload_pdptrs)
{
	if (CC(kvm_vcpu_is_illegal_gpa(vcpu, cr3)))
		return -EINVAL;

	if (reload_pdptrs && !nested_npt && is_pae_paging(vcpu) &&
	    CC(!load_pdptrs(vcpu, vcpu->arch.walk_mmu, cr3)))
		return -EINVAL;

	if (!nested_npt)
		kvm_mmu_new_pgd(vcpu, cr3);

	vcpu->arch.cr3 = cr3;
	kvm_register_mark_available(vcpu, VCPU_EXREG_CR3);

	/* Re-initialize the MMU, e.g. to pick up CR4 MMU role changes. */
	kvm_init_mmu(vcpu);

	return 0;
}