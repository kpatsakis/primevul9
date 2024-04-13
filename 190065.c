void kvm_init_shadow_npt_mmu(struct kvm_vcpu *vcpu, unsigned long cr0,
			     unsigned long cr4, u64 efer, gpa_t nested_cr3)
{
	struct kvm_mmu *context = &vcpu->arch.guest_mmu;
	struct kvm_mmu_role_regs regs = {
		.cr0 = cr0,
		.cr4 = cr4 & ~X86_CR4_PKE,
		.efer = efer,
	};
	union kvm_mmu_role new_role;

	new_role = kvm_calc_shadow_npt_root_page_role(vcpu, &regs);

	shadow_mmu_init_context(vcpu, context, &regs, new_role);
	kvm_mmu_new_pgd(vcpu, nested_cr3);
}