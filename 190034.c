kvm_calc_shadow_root_page_role_common(struct kvm_vcpu *vcpu,
				      struct kvm_mmu_role_regs *regs, bool base_only)
{
	union kvm_mmu_role role = kvm_calc_mmu_role_common(vcpu, regs, base_only);

	role.base.smep_andnot_wp = role.ext.cr4_smep && !____is_cr0_wp(regs);
	role.base.smap_andnot_wp = role.ext.cr4_smap && !____is_cr0_wp(regs);
	role.base.has_4_byte_gpte = ____is_cr0_pg(regs) && !____is_cr4_pae(regs);

	return role;
}