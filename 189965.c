static int direct_pte_prefetch_many(struct kvm_vcpu *vcpu,
				    struct kvm_mmu_page *sp,
				    u64 *start, u64 *end)
{
	struct page *pages[PTE_PREFETCH_NUM];
	struct kvm_memory_slot *slot;
	unsigned int access = sp->role.access;
	int i, ret;
	gfn_t gfn;

	gfn = kvm_mmu_page_get_gfn(sp, start - sp->spt);
	slot = gfn_to_memslot_dirty_bitmap(vcpu, gfn, access & ACC_WRITE_MASK);
	if (!slot)
		return -1;

	ret = gfn_to_page_many_atomic(slot, gfn, pages, end - start);
	if (ret <= 0)
		return -1;

	for (i = 0; i < ret; i++, gfn++, start++) {
		mmu_set_spte(vcpu, slot, start, access, gfn,
			     page_to_pfn(pages[i]), NULL);
		put_page(pages[i]);
	}

	return 0;
}