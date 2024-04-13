static bool get_mmio_spte(struct kvm_vcpu *vcpu, u64 addr, u64 *sptep)
{
	u64 sptes[PT64_ROOT_MAX_LEVEL + 1];
	struct rsvd_bits_validate *rsvd_check;
	int root, leaf, level;
	bool reserved = false;

	walk_shadow_page_lockless_begin(vcpu);

	if (is_tdp_mmu(vcpu->arch.mmu))
		leaf = kvm_tdp_mmu_get_walk(vcpu, addr, sptes, &root);
	else
		leaf = get_walk(vcpu, addr, sptes, &root);

	walk_shadow_page_lockless_end(vcpu);

	if (unlikely(leaf < 0)) {
		*sptep = 0ull;
		return reserved;
	}

	*sptep = sptes[leaf];

	/*
	 * Skip reserved bits checks on the terminal leaf if it's not a valid
	 * SPTE.  Note, this also (intentionally) skips MMIO SPTEs, which, by
	 * design, always have reserved bits set.  The purpose of the checks is
	 * to detect reserved bits on non-MMIO SPTEs. i.e. buggy SPTEs.
	 */
	if (!is_shadow_present_pte(sptes[leaf]))
		leaf++;

	rsvd_check = &vcpu->arch.mmu->shadow_zero_check;

	for (level = root; level >= leaf; level--)
		reserved |= is_rsvd_spte(rsvd_check, sptes[level], level);

	if (reserved) {
		pr_err("%s: reserved bits set on MMU-present spte, addr 0x%llx, hierarchy:\n",
		       __func__, addr);
		for (level = root; level >= leaf; level--)
			pr_err("------ spte = 0x%llx level = %d, rsvd bits = 0x%llx",
			       sptes[level], level,
			       get_rsvd_bits(rsvd_check, sptes[level], level));
	}

	return reserved;
}