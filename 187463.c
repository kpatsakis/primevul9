static void mark_proc_ids(struct mdesc_handle *hp, u64 mp, int proc_id)
{
	u64 a;

	mdesc_for_each_arc(a, hp, mp, MDESC_ARC_TYPE_BACK) {
		u64 t = mdesc_arc_target(hp, a);
		const char *name;
		const u64 *id;

		name = mdesc_node_name(hp, t);
		if (strcmp(name, "cpu"))
			continue;

		id = mdesc_get_property(hp, t, "id", NULL);
		if (*id < NR_CPUS)
			cpu_data(*id).proc_id = proc_id;
	}
}