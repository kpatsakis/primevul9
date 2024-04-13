static void set_core_ids(struct mdesc_handle *hp)
{
	int idx;
	u64 mp;

	idx = 1;

	/* Identify unique cores by looking for cpus backpointed to by
	 * level 1 instruction caches.
	 */
	mdesc_for_each_node_by_name(hp, mp, "cache") {
		const u64 *level;
		const char *type;
		int len;

		level = mdesc_get_property(hp, mp, "level", NULL);
		if (*level != 1)
			continue;

		type = mdesc_get_property(hp, mp, "type", &len);
		if (!of_find_in_proplist(type, "instn", len))
			continue;

		mark_core_ids(hp, mp, idx);
		idx++;
	}
}