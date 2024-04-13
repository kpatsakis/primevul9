check_splay(cmap_splay *tree, unsigned int node, int depth)
{
	if (node == EMPTY)
		return;
	assert(tree[node].parent == EMPTY);
	walk_splay(tree, node, do_check, tree);
}