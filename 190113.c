static void link_shadow_page(struct kvm_vcpu *vcpu, u64 *sptep,
			     struct kvm_mmu_page *sp)
{
	u64 spte;

	BUILD_BUG_ON(VMX_EPT_WRITABLE_MASK != PT_WRITABLE_MASK);

	spte = make_nonleaf_spte(sp->spt, sp_ad_disabled(sp));

	mmu_spte_set(sptep, spte);

	mmu_page_add_parent_pte(vcpu, sp, sptep);

	if (sp->unsync_children || sp->unsync)
		mark_unsync(sptep);
}