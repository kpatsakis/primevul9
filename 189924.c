static void mmu_page_remove_parent_pte(struct kvm_mmu_page *sp,
				       u64 *parent_pte)
{
	__pte_list_remove(parent_pte, &sp->parent_ptes);
}