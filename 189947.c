static void __pte_list_remove(u64 *spte, struct kvm_rmap_head *rmap_head)
{
	struct pte_list_desc *desc;
	struct pte_list_desc *prev_desc;
	int i;

	if (!rmap_head->val) {
		pr_err("%s: %p 0->BUG\n", __func__, spte);
		BUG();
	} else if (!(rmap_head->val & 1)) {
		rmap_printk("%p 1->0\n", spte);
		if ((u64 *)rmap_head->val != spte) {
			pr_err("%s:  %p 1->BUG\n", __func__, spte);
			BUG();
		}
		rmap_head->val = 0;
	} else {
		rmap_printk("%p many->many\n", spte);
		desc = (struct pte_list_desc *)(rmap_head->val & ~1ul);
		prev_desc = NULL;
		while (desc) {
			for (i = 0; i < desc->spte_count; ++i) {
				if (desc->sptes[i] == spte) {
					pte_list_desc_remove_entry(rmap_head,
							desc, i, prev_desc);
					return;
				}
			}
			prev_desc = desc;
			desc = desc->more;
		}
		pr_err("%s: %p many->many\n", __func__, spte);
		BUG();
	}
}