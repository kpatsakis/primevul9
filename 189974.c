unsigned int pte_list_count(struct kvm_rmap_head *rmap_head)
{
	struct pte_list_desc *desc;
	unsigned int count = 0;

	if (!rmap_head->val)
		return 0;
	else if (!(rmap_head->val & 1))
		return 1;

	desc = (struct pte_list_desc *)(rmap_head->val & ~1ul);

	while (desc) {
		count += desc->spte_count;
		desc = desc->more;
	}

	return count;
}