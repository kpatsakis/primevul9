kvm_calc_shadow_mmu_root_page_role(struct kvm_vcpu *vcpu,
				   struct kvm_mmu_role_regs *regs, bool base_only)
{
	union kvm_mmu_role role =
		kvm_calc_shadow_root_page_role_common(vcpu, regs, base_only);

	role.base.direct = !____is_cr0_pg(regs);

	if (!____is_efer_lma(regs))
		role.base.level = PT32E_ROOT_LEVEL;
	else if (____is_cr4_la57(regs))
		role.base.level = PT64_ROOT_5LEVEL;
	else
		role.base.level = PT64_ROOT_4LEVEL;

	return role;
}