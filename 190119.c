static u64 *fast_pf_get_last_sptep(struct kvm_vcpu *vcpu, gpa_t gpa, u64 *spte)
{
	struct kvm_shadow_walk_iterator iterator;
	u64 old_spte;
	u64 *sptep = NULL;

	for_each_shadow_entry_lockless(vcpu, gpa, iterator, old_spte) {
		sptep = iterator.sptep;
		*spte = old_spte;
	}

	return sptep;
}