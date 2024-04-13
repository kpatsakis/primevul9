static void rmap_remove(struct kvm *kvm, u64 *spte)
{
	struct kvm_memslots *slots;
	struct kvm_memory_slot *slot;
	struct kvm_mmu_page *sp;
	gfn_t gfn;
	struct kvm_rmap_head *rmap_head;

	sp = sptep_to_sp(spte);
	gfn = kvm_mmu_page_get_gfn(sp, spte - sp->spt);

	/*
	 * Unlike rmap_add, rmap_remove does not run in the context of a vCPU
	 * so we have to determine which memslots to use based on context
	 * information in sp->role.
	 */
	slots = kvm_memslots_for_spte_role(kvm, sp->role);

	slot = __gfn_to_memslot(slots, gfn);
	rmap_head = gfn_to_rmap(gfn, sp->role.level, slot);

	__pte_list_remove(spte, rmap_head);
}