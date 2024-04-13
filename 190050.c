static bool pte_list_destroy(struct kvm *kvm, struct kvm_rmap_head *rmap_head)
{
	struct pte_list_desc *desc, *next;
	int i;

	if (!rmap_head->val)
		return false;

	if (!(rmap_head->val & 1)) {
		mmu_spte_clear_track_bits(kvm, (u64 *)rmap_head->val);
		goto out;
	}

	desc = (struct pte_list_desc *)(rmap_head->val & ~1ul);

	for (; desc; desc = next) {
		for (i = 0; i < desc->spte_count; i++)
			mmu_spte_clear_track_bits(kvm, desc->sptes[i]);
		next = desc->more;
		mmu_free_pte_list_desc(desc);
	}
out:
	/* rmap_head is meaningless now, remember to reset it */
	rmap_head->val = 0;
	return true;
}