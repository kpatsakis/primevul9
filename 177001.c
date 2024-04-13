static noinline void reada_for_balance(struct btrfs_path *path, int level)
{
	struct extent_buffer *parent;
	int slot;
	int nritems;

	parent = path->nodes[level + 1];
	if (!parent)
		return;

	nritems = btrfs_header_nritems(parent);
	slot = path->slots[level + 1];

	if (slot > 0)
		btrfs_readahead_node_child(parent, slot - 1);
	if (slot + 1 < nritems)
		btrfs_readahead_node_child(parent, slot + 1);
}