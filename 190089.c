void disallowed_hugepage_adjust(struct kvm_page_fault *fault, u64 spte, int cur_level)
{
	if (cur_level > PG_LEVEL_4K &&
	    cur_level == fault->goal_level &&
	    is_shadow_present_pte(spte) &&
	    !is_large_pte(spte)) {
		/*
		 * A small SPTE exists for this pfn, but FNAME(fetch)
		 * and __direct_map would like to create a large PTE
		 * instead: just force them to go down another level,
		 * patching back for them into pfn the next 9 bits of
		 * the address.
		 */
		u64 page_mask = KVM_PAGES_PER_HPAGE(cur_level) -
				KVM_PAGES_PER_HPAGE(cur_level - 1);
		fault->pfn |= fault->gfn & page_mask;
		fault->goal_level--;
	}
}