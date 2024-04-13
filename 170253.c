do_check(cmap_splay *node, void *arg)
{
	cmap_splay *tree = arg;
	unsigned int num = node - tree;
	assert(node->left == EMPTY || tree[node->left].parent == num);
	assert(node->right == EMPTY || tree[node->right].parent == num);
}