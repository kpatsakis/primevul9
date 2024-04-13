static void drop_parent_pte(struct kvm_mmu_page *sp,
			    u64 *parent_pte)
{
	mmu_page_remove_parent_pte(sp, parent_pte);
	mmu_spte_clear_no_track(parent_pte);
}