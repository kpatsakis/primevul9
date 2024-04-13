static int get_walk(struct kvm_vcpu *vcpu, u64 addr, u64 *sptes, int *root_level)
{
	struct kvm_shadow_walk_iterator iterator;
	int leaf = -1;
	u64 spte;

	for (shadow_walk_init(&iterator, vcpu, addr),
	     *root_level = iterator.level;
	     shadow_walk_okay(&iterator);
	     __shadow_walk_next(&iterator, spte)) {
		leaf = iterator.level;
		spte = mmu_spte_get_lockless(iterator.sptep);

		sptes[leaf] = spte;
	}

	return leaf;
}