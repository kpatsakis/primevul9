char *device_node_gen_full_name(const struct device_node *np, char *buf, char *end)
{
	int depth;
	const struct device_node *parent = np->parent;

	/* special case for root node */
	if (!parent)
		return string(buf, end, "/", default_str_spec);

	for (depth = 0; parent->parent; depth++)
		parent = parent->parent;

	for ( ; depth >= 0; depth--) {
		buf = string(buf, end, "/", default_str_spec);
		buf = string(buf, end, device_node_name_for_depth(np, depth),
			     default_str_spec);
	}
	return buf;
}