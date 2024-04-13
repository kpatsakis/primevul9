static void shadow_page_table_clear_flood(struct kvm_vcpu *vcpu, gva_t addr)
{
	struct kvm_shadow_walk_iterator iterator;
	u64 spte;

	walk_shadow_page_lockless_begin(vcpu);
	for_each_shadow_entry_lockless(vcpu, addr, iterator, spte)
		clear_sp_write_flooding_count(iterator.sptep);
	walk_shadow_page_lockless_end(vcpu);
}