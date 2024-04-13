static bool slot_rmap_walk_okay(struct slot_rmap_walk_iterator *iterator)
{
	return !!iterator->rmap;
}