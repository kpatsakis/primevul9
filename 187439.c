static void __mark_core_id(struct mdesc_handle *hp, u64 node,
			   int core_id)
{
	const u64 *id = mdesc_get_property(hp, node, "id", NULL);

	if (*id < num_possible_cpus())
		cpu_data(*id).core_id = core_id;
}