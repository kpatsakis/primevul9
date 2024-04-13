static void kvm_init_shadow_mmu(struct kvm_vcpu *vcpu,
				struct kvm_mmu_role_regs *regs)
{
	struct kvm_mmu *context = &vcpu->arch.root_mmu;
	union kvm_mmu_role new_role =
		kvm_calc_shadow_mmu_root_page_role(vcpu, regs, false);

	shadow_mmu_init_context(vcpu, context, regs, new_role);
}