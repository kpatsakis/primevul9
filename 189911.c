kvm_calc_tdp_mmu_root_page_role(struct kvm_vcpu *vcpu,
				struct kvm_mmu_role_regs *regs, bool base_only)
{
	union kvm_mmu_role role = kvm_calc_mmu_role_common(vcpu, regs, base_only);

	role.base.ad_disabled = (shadow_accessed_mask == 0);
	role.base.level = kvm_mmu_get_tdp_level(vcpu);
	role.base.direct = true;
	role.base.has_4_byte_gpte = false;

	return role;
}