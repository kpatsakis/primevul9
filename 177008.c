void setup_items_for_insert(struct btrfs_root *root, struct btrfs_path *path,
			    const struct btrfs_key *cpu_key, u32 *data_size,
			    int nr)
{
	struct btrfs_fs_info *fs_info = root->fs_info;
	struct btrfs_item *item;
	int i;
	u32 nritems;
	unsigned int data_end;
	struct btrfs_disk_key disk_key;
	struct extent_buffer *leaf;
	int slot;
	struct btrfs_map_token token;
	u32 total_size;
	u32 total_data = 0;

	for (i = 0; i < nr; i++)
		total_data += data_size[i];
	total_size = total_data + (nr * sizeof(struct btrfs_item));

	if (path->slots[0] == 0) {
		btrfs_cpu_key_to_disk(&disk_key, cpu_key);
		fixup_low_keys(path, &disk_key, 1);
	}
	btrfs_unlock_up_safe(path, 1);

	leaf = path->nodes[0];
	slot = path->slots[0];

	nritems = btrfs_header_nritems(leaf);
	data_end = leaf_data_end(leaf);

	if (btrfs_leaf_free_space(leaf) < total_size) {
		btrfs_print_leaf(leaf);
		btrfs_crit(fs_info, "not enough freespace need %u have %d",
			   total_size, btrfs_leaf_free_space(leaf));
		BUG();
	}

	btrfs_init_map_token(&token, leaf);
	if (slot != nritems) {
		unsigned int old_data = btrfs_item_end_nr(leaf, slot);

		if (old_data < data_end) {
			btrfs_print_leaf(leaf);
			btrfs_crit(fs_info,
		"item at slot %d with data offset %u beyond data end of leaf %u",
				   slot, old_data, data_end);
			BUG();
		}
		/*
		 * item0..itemN ... dataN.offset..dataN.size .. data0.size
		 */
		/* first correct the data pointers */
		for (i = slot; i < nritems; i++) {
			u32 ioff;

			item = btrfs_item_nr(i);
			ioff = btrfs_token_item_offset(&token, item);
			btrfs_set_token_item_offset(&token, item,
						    ioff - total_data);
		}
		/* shift the items */
		memmove_extent_buffer(leaf, btrfs_item_nr_offset(slot + nr),
			      btrfs_item_nr_offset(slot),
			      (nritems - slot) * sizeof(struct btrfs_item));

		/* shift the data */
		memmove_extent_buffer(leaf, BTRFS_LEAF_DATA_OFFSET +
			      data_end - total_data, BTRFS_LEAF_DATA_OFFSET +
			      data_end, old_data - data_end);
		data_end = old_data;
	}

	/* setup the item for the new data */
	for (i = 0; i < nr; i++) {
		btrfs_cpu_key_to_disk(&disk_key, cpu_key + i);
		btrfs_set_item_key(leaf, &disk_key, slot + i);
		item = btrfs_item_nr(slot + i);
		data_end -= data_size[i];
		btrfs_set_token_item_offset(&token, item, data_end);
		btrfs_set_token_item_size(&token, item, data_size[i]);
	}

	btrfs_set_header_nritems(leaf, nritems + nr);
	btrfs_mark_buffer_dirty(leaf);

	if (btrfs_leaf_free_space(leaf) < 0) {
		btrfs_print_leaf(leaf);
		BUG();
	}
}