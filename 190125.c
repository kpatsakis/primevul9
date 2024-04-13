static int mmu_topup_memory_caches(struct kvm_vcpu *vcpu, bool maybe_indirect)
{
	int r;

	/* 1 rmap, 1 parent PTE per level, and the prefetched rmaps. */
	r = kvm_mmu_topup_memory_cache(&vcpu->arch.mmu_pte_list_desc_cache,
				       1 + PT64_ROOT_MAX_LEVEL + PTE_PREFETCH_NUM);
	if (r)
		return r;
	r = kvm_mmu_topup_memory_cache(&vcpu->arch.mmu_shadow_page_cache,
				       PT64_ROOT_MAX_LEVEL);
	if (r)
		return r;
	if (maybe_indirect) {
		r = kvm_mmu_topup_memory_cache(&vcpu->arch.mmu_gfn_array_cache,
					       PT64_ROOT_MAX_LEVEL);
		if (r)
			return r;
	}
	return kvm_mmu_topup_memory_cache(&vcpu->arch.mmu_page_header_cache,
					  PT64_ROOT_MAX_LEVEL);
}