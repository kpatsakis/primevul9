rmap_walk_init_level(struct slot_rmap_walk_iterator *iterator, int level)
{
	iterator->level = level;
	iterator->gfn = iterator->start_gfn;
	iterator->rmap = gfn_to_rmap(iterator->gfn, level, iterator->slot);
	iterator->end_rmap = gfn_to_rmap(iterator->end_gfn, level, iterator->slot);
}