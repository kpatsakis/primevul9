static void init_kvm_nested_mmu(struct kvm_vcpu *vcpu)
{
	struct kvm_mmu_role_regs regs = vcpu_to_role_regs(vcpu);
	union kvm_mmu_role new_role = kvm_calc_nested_mmu_role(vcpu, &regs);
	struct kvm_mmu *g_context = &vcpu->arch.nested_mmu;

	if (new_role.as_u64 == g_context->mmu_role.as_u64)
		return;

	g_context->mmu_role.as_u64 = new_role.as_u64;
	g_context->get_guest_pgd     = get_cr3;
	g_context->get_pdptr         = kvm_pdptr_read;
	g_context->inject_page_fault = kvm_inject_page_fault;
	g_context->root_level        = new_role.base.level;

	/*
	 * L2 page tables are never shadowed, so there is no need to sync
	 * SPTEs.
	 */
	g_context->invlpg            = NULL;

	/*
	 * Note that arch.mmu->gva_to_gpa translates l2_gpa to l1_gpa using
	 * L1's nested page tables (e.g. EPT12). The nested translation
	 * of l2_gva to l1_gpa is done by arch.nested_mmu.gva_to_gpa using
	 * L2's page tables as the first level of translation and L1's
	 * nested page tables as the second level of translation. Basically
	 * the gva_to_gpa functions between mmu and nested_mmu are swapped.
	 */
	if (!is_paging(vcpu))
		g_context->gva_to_gpa = nonpaging_gva_to_gpa;
	else if (is_long_mode(vcpu))
		g_context->gva_to_gpa = paging64_gva_to_gpa;
	else if (is_pae(vcpu))
		g_context->gva_to_gpa = paging64_gva_to_gpa;
	else
		g_context->gva_to_gpa = paging32_gva_to_gpa;

	reset_guest_paging_metadata(vcpu, g_context);
}