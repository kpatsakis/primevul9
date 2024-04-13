int kvm_mmu_max_mapping_level(struct kvm *kvm,
			      const struct kvm_memory_slot *slot, gfn_t gfn,
			      kvm_pfn_t pfn, int max_level)
{
	struct kvm_lpage_info *linfo;
	int host_level;

	max_level = min(max_level, max_huge_page_level);
	for ( ; max_level > PG_LEVEL_4K; max_level--) {
		linfo = lpage_info_slot(gfn, slot, max_level);
		if (!linfo->disallow_lpage)
			break;
	}

	if (max_level == PG_LEVEL_4K)
		return PG_LEVEL_4K;

	host_level = host_pfn_mapping_level(kvm, gfn, pfn, slot);
	return min(host_level, max_level);
}