static union kvm_mmu_role kvm_calc_mmu_role_common(struct kvm_vcpu *vcpu,
						   struct kvm_mmu_role_regs *regs,
						   bool base_only)
{
	union kvm_mmu_role role = {0};

	role.base.access = ACC_ALL;
	if (____is_cr0_pg(regs)) {
		role.base.efer_nx = ____is_efer_nx(regs);
		role.base.cr0_wp = ____is_cr0_wp(regs);
	}
	role.base.smm = is_smm(vcpu);
	role.base.guest_mode = is_guest_mode(vcpu);

	if (base_only)
		return role;

	role.ext = kvm_calc_mmu_role_ext(vcpu, regs);

	return role;
}