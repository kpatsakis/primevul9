static void __mark_max_cache_id(struct mdesc_handle *hp, u64 node,
				int max_cache_id)
{
	const u64 *id = mdesc_get_property(hp, node, "id", NULL);

	if (*id < num_possible_cpus()) {
		cpu_data(*id).max_cache_id = max_cache_id;

		/**
		 * On systems without explicit socket descriptions socket
		 * is max_cache_id
		 */
		cpu_data(*id).sock_id = max_cache_id;
	}
}