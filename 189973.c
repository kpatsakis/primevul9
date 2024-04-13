pte_list_desc_remove_entry(struct kvm_rmap_head *rmap_head,
			   struct pte_list_desc *desc, int i,
			   struct pte_list_desc *prev_desc)
{
	int j = desc->spte_count - 1;

	desc->sptes[i] = desc->sptes[j];
	desc->sptes[j] = NULL;
	desc->spte_count--;
	if (desc->spte_count)
		return;
	if (!prev_desc && !desc->more)
		rmap_head->val = 0;
	else
		if (prev_desc)
			prev_desc->more = desc->more;
		else
			rmap_head->val = (unsigned long)desc->more | 1;
	mmu_free_pte_list_desc(desc);
}