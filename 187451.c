static void set_sock_ids_by_socket(struct mdesc_handle *hp, u64 mp)
{
	int idx = 1;

	mdesc_for_each_node_by_name(hp, mp, "socket") {
		u64 a;

		mdesc_for_each_arc(a, hp, mp, MDESC_ARC_TYPE_FWD) {
			u64 t = mdesc_arc_target(hp, a);
			const char *name;
			const u64 *id;

			name = mdesc_node_name(hp, t);
			if (strcmp(name, "cpu"))
				continue;

			id = mdesc_get_property(hp, t, "id", NULL);
			if (*id < num_possible_cpus())
				cpu_data(*id).sock_id = idx;
		}
		idx++;
	}
}