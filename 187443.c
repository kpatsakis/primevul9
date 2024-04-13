static void __set_proc_ids(struct mdesc_handle *hp, const char *exec_unit_name)
{
	int idx;
	u64 mp;

	idx = 0;
	mdesc_for_each_node_by_name(hp, mp, exec_unit_name) {
		const char *type;
		int len;

		type = mdesc_get_property(hp, mp, "type", &len);
		if (!of_find_in_proplist(type, "int", len) &&
		    !of_find_in_proplist(type, "integer", len))
			continue;

		mark_proc_ids(hp, mp, idx);
		idx++;
	}
}