count_node_types(cmap_splay *node, void *arg)
{
	int *counts = (int *)arg;

	if (node->many)
		counts[2]++;
	else if (node->low <= 0xffff && node->high <= 0xFFFF && node->out <= 0xFFFF)
		counts[0]++;
	else
		counts[1]++;
}