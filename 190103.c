kvm_calc_shadow_npt_root_page_role(struct kvm_vcpu *vcpu,
				   struct kvm_mmu_role_regs *regs)
{
	union kvm_mmu_role role =
		kvm_calc_shadow_root_page_role_common(vcpu, regs, false);

	role.base.direct = false;
	role.base.level = kvm_mmu_get_tdp_level(vcpu);

	return role;
}