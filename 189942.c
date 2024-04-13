slot_handle_level(struct kvm *kvm, const struct kvm_memory_slot *memslot,
		  slot_level_handler fn, int start_level, int end_level,
		  bool flush_on_yield)
{
	return slot_handle_level_range(kvm, memslot, fn, start_level,
			end_level, memslot->base_gfn,
			memslot->base_gfn + memslot->npages - 1,
			flush_on_yield, false);
}