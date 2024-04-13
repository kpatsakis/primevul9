void kvm_init_shadow_ept_mmu(struct kvm_vcpu *vcpu, bool execonly,
			     int huge_page_level, bool accessed_dirty,
			     gpa_t new_eptp)
{
	struct kvm_mmu *context = &vcpu->arch.guest_mmu;
	u8 level = vmx_eptp_page_walk_level(new_eptp);
	union kvm_mmu_role new_role =
		kvm_calc_shadow_ept_root_page_role(vcpu, accessed_dirty,
						   execonly, level);

	if (new_role.as_u64 != context->mmu_role.as_u64) {
		context->mmu_role.as_u64 = new_role.as_u64;

		context->shadow_root_level = level;

		context->ept_ad = accessed_dirty;
		context->page_fault = ept_page_fault;
		context->gva_to_gpa = ept_gva_to_gpa;
		context->sync_page = ept_sync_page;
		context->invlpg = ept_invlpg;
		context->root_level = level;
		context->direct_map = false;
		update_permission_bitmask(context, true);
		context->pkru_mask = 0;
		reset_rsvds_bits_mask_ept(vcpu, context, execonly, huge_page_level);
		reset_ept_shadow_zero_bits_mask(context, execonly);
	}

	kvm_mmu_new_pgd(vcpu, new_eptp);
}