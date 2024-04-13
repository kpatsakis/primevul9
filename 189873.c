static void account_shadowed(struct kvm *kvm, struct kvm_mmu_page *sp)
{
	struct kvm_memslots *slots;
	struct kvm_memory_slot *slot;
	gfn_t gfn;

	kvm->arch.indirect_shadow_pages++;
	gfn = sp->gfn;
	slots = kvm_memslots_for_spte_role(kvm, sp->role);
	slot = __gfn_to_memslot(slots, gfn);

	/* the non-leaf shadow pages are keeping readonly. */
	if (sp->role.level > PG_LEVEL_4K)
		return kvm_slot_page_track_add_page(kvm, slot, gfn,
						    KVM_PAGE_TRACK_WRITE);

	kvm_mmu_gfn_disallow_lpage(slot, gfn);
}