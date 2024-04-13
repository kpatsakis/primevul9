slot_rmap_walk_init(struct slot_rmap_walk_iterator *iterator,
		    const struct kvm_memory_slot *slot, int start_level,
		    int end_level, gfn_t start_gfn, gfn_t end_gfn)
{
	iterator->slot = slot;
	iterator->start_level = start_level;
	iterator->end_level = end_level;
	iterator->start_gfn = start_gfn;
	iterator->end_gfn = end_gfn;

	rmap_walk_init_level(iterator, iterator->start_level);
}