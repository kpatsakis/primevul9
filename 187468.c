static void rel_vdev_port_node_info(union md_node_info *node_info)
{
	if (node_info && node_info->vdev_port.name) {
		kfree_const(node_info->vdev_port.name);
		node_info->vdev_port.name = NULL;
	}
}