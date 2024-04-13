slot_handle_level_4k(struct kvm *kvm, const struct kvm_memory_slot *memslot,
		     slot_level_handler fn, bool flush_on_yield)
{
	return slot_handle_level(kvm, memslot, fn, PG_LEVEL_4K,
				 PG_LEVEL_4K, flush_on_yield);
}