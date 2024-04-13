static void init_kvm_tdp_mmu(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu *context = &vcpu->arch.root_mmu;
	struct kvm_mmu_role_regs regs = vcpu_to_role_regs(vcpu);
	union kvm_mmu_role new_role =
		kvm_calc_tdp_mmu_root_page_role(vcpu, &regs, false);

	if (new_role.as_u64 == context->mmu_role.as_u64)
		return;

	context->mmu_role.as_u64 = new_role.as_u64;
	context->page_fault = kvm_tdp_page_fault;
	context->sync_page = nonpaging_sync_page;
	context->invlpg = NULL;
	context->shadow_root_level = kvm_mmu_get_tdp_level(vcpu);
	context->direct_map = true;
	context->get_guest_pgd = get_cr3;
	context->get_pdptr = kvm_pdptr_read;
	context->inject_page_fault = kvm_inject_page_fault;
	context->root_level = role_regs_to_root_level(&regs);

	if (!is_cr0_pg(context))
		context->gva_to_gpa = nonpaging_gva_to_gpa;
	else if (is_cr4_pae(context))
		context->gva_to_gpa = paging64_gva_to_gpa;
	else
		context->gva_to_gpa = paging32_gva_to_gpa;

	reset_guest_paging_metadata(vcpu, context);
	reset_tdp_shadow_zero_bits_mask(context);
}