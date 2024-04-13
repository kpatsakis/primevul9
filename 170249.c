static void walk_splay(cmap_splay *tree, unsigned int node, void (*fn)(cmap_splay *, void *), void *arg)
{
	int from = TOP;

	while (node != EMPTY)
	{
		switch (from)
		{
		case TOP:
			if (tree[node].left != EMPTY)
			{
				node = tree[node].left;
				from = TOP;
				break;
			}
			/* fallthrough */
		case LEFT:
			fn(&tree[node], arg);
			if (tree[node].right != EMPTY)
			{
				node = tree[node].right;
				from = TOP;
				break;
			}
			/* fallthrough */
		case RIGHT:
			{
				unsigned int parent = tree[node].parent;
				if (parent == EMPTY)
					return;
				if (tree[parent].left == node)
					from = LEFT;
				else
				{
					assert(tree[parent].right == node);
					from = RIGHT;
				}
				node = parent;
			}
		}
	}
}