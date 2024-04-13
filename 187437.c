static const u64 *parent_cfg_handle(struct mdesc_handle *hp, u64 node)
{
	const u64 *id;
	u64 a;

	id = NULL;
	mdesc_for_each_arc(a, hp, node, MDESC_ARC_TYPE_BACK) {
		u64 target;

		target = mdesc_arc_target(hp, a);
		id = mdesc_get_property(hp, target,
					"cfg-handle", NULL);
		if (id)
			break;
	}

	return id;
}