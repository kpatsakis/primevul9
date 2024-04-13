static bool vdev_port_node_match(union md_node_info *a_node_info,
				 union md_node_info *b_node_info)
{
	if (a_node_info->vdev_port.id != b_node_info->vdev_port.id)
		return false;

	if (a_node_info->vdev_port.parent_cfg_hdl !=
	    b_node_info->vdev_port.parent_cfg_hdl)
		return false;

	if (strncmp(a_node_info->vdev_port.name,
		    b_node_info->vdev_port.name, MDESC_MAX_STR_LEN) != 0)
		return false;

	return true;
}