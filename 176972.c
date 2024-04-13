struct extent_buffer *btrfs_read_node_slot(struct extent_buffer *parent,
					   int slot)
{
	int level = btrfs_header_level(parent);
	struct extent_buffer *eb;
	struct btrfs_key first_key;

	if (slot < 0 || slot >= btrfs_header_nritems(parent))
		return ERR_PTR(-ENOENT);

	BUG_ON(level == 0);

	btrfs_node_key_to_cpu(parent, &first_key, slot);
	eb = read_tree_block(parent->fs_info, btrfs_node_blockptr(parent, slot),
			     btrfs_header_owner(parent),
			     btrfs_node_ptr_generation(parent, slot),
			     level - 1, &first_key);
	if (!IS_ERR(eb) && !extent_buffer_uptodate(eb)) {
		free_extent_buffer(eb);
		eb = ERR_PTR(-EIO);
	}

	return eb;
}