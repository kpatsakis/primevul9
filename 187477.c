static int get_ds_port_node_info(struct mdesc_handle *md, u64 node,
				 union md_node_info *node_info)
{
	const u64 *idp;

	/* DS port nodes use the "id" property to distinguish them */
	idp = mdesc_get_property(md, node, "id", NULL);
	if (!idp)
		return -1;

	node_info->ds_port.id = *idp;

	return 0;
}