kvm_calc_nested_mmu_role(struct kvm_vcpu *vcpu, struct kvm_mmu_role_regs *regs)
{
	union kvm_mmu_role role;

	role = kvm_calc_shadow_root_page_role_common(vcpu, regs, false);

	/*
	 * Nested MMUs are used only for walking L2's gva->gpa, they never have
	 * shadow pages of their own and so "direct" has no meaning.   Set it
	 * to "true" to try to detect bogus usage of the nested MMU.
	 */
	role.base.direct = true;
	role.base.level = role_regs_to_root_level(regs);
	return role;
}