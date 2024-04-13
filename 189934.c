kvm_calc_shadow_ept_root_page_role(struct kvm_vcpu *vcpu, bool accessed_dirty,
				   bool execonly, u8 level)
{
	union kvm_mmu_role role = {0};

	/* SMM flag is inherited from root_mmu */
	role.base.smm = vcpu->arch.root_mmu.mmu_role.base.smm;

	role.base.level = level;
	role.base.has_4_byte_gpte = false;
	role.base.direct = false;
	role.base.ad_disabled = !accessed_dirty;
	role.base.guest_mode = true;
	role.base.access = ACC_ALL;

	/* EPT, and thus nested EPT, does not consume CR0, CR4, nor EFER. */
	role.ext.word = 0;
	role.ext.execonly = execonly;
	role.ext.valid = 1;

	return role;
}