static void mark_core_ids(struct mdesc_handle *hp, u64 mp,
			  int core_id)
{
	find_back_node_value(hp, mp, "cpu", __mark_core_id, core_id, 10);
}