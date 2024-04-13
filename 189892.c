static int pte_list_add(struct kvm_vcpu *vcpu, u64 *spte,
			struct kvm_rmap_head *rmap_head)
{
	struct pte_list_desc *desc;
	int count = 0;

	if (!rmap_head->val) {
		rmap_printk("%p %llx 0->1\n", spte, *spte);
		rmap_head->val = (unsigned long)spte;
	} else if (!(rmap_head->val & 1)) {
		rmap_printk("%p %llx 1->many\n", spte, *spte);
		desc = mmu_alloc_pte_list_desc(vcpu);
		desc->sptes[0] = (u64 *)rmap_head->val;
		desc->sptes[1] = spte;
		desc->spte_count = 2;
		rmap_head->val = (unsigned long)desc | 1;
		++count;
	} else {
		rmap_printk("%p %llx many->many\n", spte, *spte);
		desc = (struct pte_list_desc *)(rmap_head->val & ~1ul);
		while (desc->spte_count == PTE_LIST_EXT) {
			count += PTE_LIST_EXT;
			if (!desc->more) {
				desc->more = mmu_alloc_pte_list_desc(vcpu);
				desc = desc->more;
				desc->spte_count = 0;
				break;
			}
			desc = desc->more;
		}
		count += desc->spte_count;
		desc->sptes[desc->spte_count++] = spte;
	}
	return count;
}