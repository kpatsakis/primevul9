static void fill_in_one_cache(cpuinfo_sparc *c, struct mdesc_handle *hp, u64 mp)
{
	const u64 *level = mdesc_get_property(hp, mp, "level", NULL);
	const u64 *size = mdesc_get_property(hp, mp, "size", NULL);
	const u64 *line_size = mdesc_get_property(hp, mp, "line-size", NULL);
	const char *type;
	int type_len;

	type = mdesc_get_property(hp, mp, "type", &type_len);

	switch (*level) {
	case 1:
		if (of_find_in_proplist(type, "instn", type_len)) {
			c->icache_size = *size;
			c->icache_line_size = *line_size;
		} else if (of_find_in_proplist(type, "data", type_len)) {
			c->dcache_size = *size;
			c->dcache_line_size = *line_size;
		}
		break;

	case 2:
		c->ecache_size = *size;
		c->ecache_line_size = *line_size;
		break;

	default:
		break;
	}

	if (*level == 1) {
		u64 a;

		mdesc_for_each_arc(a, hp, mp, MDESC_ARC_TYPE_FWD) {
			u64 target = mdesc_arc_target(hp, a);
			const char *name = mdesc_node_name(hp, target);

			if (!strcmp(name, "cache"))
				fill_in_one_cache(c, hp, target);
		}
	}
}