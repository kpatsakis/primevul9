static bool ds_port_node_match(union md_node_info *a_node_info,
			       union md_node_info *b_node_info)
{
	if (a_node_info->ds_port.id != b_node_info->ds_port.id)
		return false;

	return true;
}