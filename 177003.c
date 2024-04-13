void btrfs_extend_item(struct btrfs_path *path, u32 data_size)
{
	int slot;
	struct extent_buffer *leaf;
	struct btrfs_item *item;
	u32 nritems;
	unsigned int data_end;
	unsigned int old_data;
	unsigned int old_size;
	int i;
	struct btrfs_map_token token;

	leaf = path->nodes[0];

	nritems = btrfs_header_nritems(leaf);
	data_end = leaf_data_end(leaf);

	if (btrfs_leaf_free_space(leaf) < data_size) {
		btrfs_print_leaf(leaf);
		BUG();
	}
	slot = path->slots[0];
	old_data = btrfs_item_end_nr(leaf, slot);

	BUG_ON(slot < 0);
	if (slot >= nritems) {
		btrfs_print_leaf(leaf);
		btrfs_crit(leaf->fs_info, "slot %d too large, nritems %d",
			   slot, nritems);
		BUG();
	}

	/*
	 * item0..itemN ... dataN.offset..dataN.size .. data0.size
	 */
	/* first correct the data pointers */
	btrfs_init_map_token(&token, leaf);
	for (i = slot; i < nritems; i++) {
		u32 ioff;
		item = btrfs_item_nr(i);

		ioff = btrfs_token_item_offset(&token, item);
		btrfs_set_token_item_offset(&token, item, ioff - data_size);
	}

	/* shift the data */
	memmove_extent_buffer(leaf, BTRFS_LEAF_DATA_OFFSET +
		      data_end - data_size, BTRFS_LEAF_DATA_OFFSET +
		      data_end, old_data - data_end);

	data_end = old_data;
	old_size = btrfs_item_size_nr(leaf, slot);
	item = btrfs_item_nr(slot);
	btrfs_set_item_size(leaf, item, old_size + data_size);
	btrfs_mark_buffer_dirty(leaf);

	if (btrfs_leaf_free_space(leaf) < 0) {
		btrfs_print_leaf(leaf);
		BUG();
	}
}