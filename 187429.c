static void mark_max_cache_ids(struct mdesc_handle *hp, u64 mp,
			       int max_cache_id)
{
	find_back_node_value(hp, mp, "cpu", __mark_max_cache_id,
			     max_cache_id, 10);
}