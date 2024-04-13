static void set_sock_ids(struct mdesc_handle *hp)
{
	u64 mp;

	/**
	 * Find the highest level of shared cache which pre-T7 is also
	 * the socket.
	 */
	if (!set_max_cache_ids_by_cache(hp, 3))
		set_max_cache_ids_by_cache(hp, 2);

	/* If machine description exposes sockets data use it.*/
	mp = mdesc_node_by_name(hp, MDESC_NODE_NULL, "sockets");
	if (mp != MDESC_NODE_NULL)
		set_sock_ids_by_socket(hp, mp);
}