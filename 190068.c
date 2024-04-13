static void shadow_mmu_init_context(struct kvm_vcpu *vcpu, struct kvm_mmu *context,
				    struct kvm_mmu_role_regs *regs,
				    union kvm_mmu_role new_role)
{
	if (new_role.as_u64 == context->mmu_role.as_u64)
		return;

	context->mmu_role.as_u64 = new_role.as_u64;

	if (!is_cr0_pg(context))
		nonpaging_init_context(context);
	else if (is_cr4_pae(context))
		paging64_init_context(context);
	else
		paging32_init_context(context);
	context->root_level = role_regs_to_root_level(regs);

	reset_guest_paging_metadata(vcpu, context);
	context->shadow_root_level = new_role.base.level;

	reset_shadow_zero_bits_mask(vcpu, context);
}