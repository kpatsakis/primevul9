static noinline int __push_leaf_right(struct btrfs_path *path,
				      int data_size, int empty,
				      struct extent_buffer *right,
				      int free_space, u32 left_nritems,
				      u32 min_slot)
{
	struct btrfs_fs_info *fs_info = right->fs_info;
	struct extent_buffer *left = path->nodes[0];
	struct extent_buffer *upper = path->nodes[1];
	struct btrfs_map_token token;
	struct btrfs_disk_key disk_key;
	int slot;
	u32 i;
	int push_space = 0;
	int push_items = 0;
	struct btrfs_item *item;
	u32 nr;
	u32 right_nritems;
	u32 data_end;
	u32 this_item_size;

	if (empty)
		nr = 0;
	else
		nr = max_t(u32, 1, min_slot);

	if (path->slots[0] >= left_nritems)
		push_space += data_size;

	slot = path->slots[1];
	i = left_nritems - 1;
	while (i >= nr) {
		item = btrfs_item_nr(i);

		if (!empty && push_items > 0) {
			if (path->slots[0] > i)
				break;
			if (path->slots[0] == i) {
				int space = btrfs_leaf_free_space(left);

				if (space + push_space * 2 > free_space)
					break;
			}
		}

		if (path->slots[0] == i)
			push_space += data_size;

		this_item_size = btrfs_item_size(left, item);
		if (this_item_size + sizeof(*item) + push_space > free_space)
			break;

		push_items++;
		push_space += this_item_size + sizeof(*item);
		if (i == 0)
			break;
		i--;
	}

	if (push_items == 0)
		goto out_unlock;

	WARN_ON(!empty && push_items == left_nritems);

	/* push left to right */
	right_nritems = btrfs_header_nritems(right);

	push_space = btrfs_item_end_nr(left, left_nritems - push_items);
	push_space -= leaf_data_end(left);

	/* make room in the right data area */
	data_end = leaf_data_end(right);
	memmove_extent_buffer(right,
			      BTRFS_LEAF_DATA_OFFSET + data_end - push_space,
			      BTRFS_LEAF_DATA_OFFSET + data_end,
			      BTRFS_LEAF_DATA_SIZE(fs_info) - data_end);

	/* copy from the left data area */
	copy_extent_buffer(right, left, BTRFS_LEAF_DATA_OFFSET +
		     BTRFS_LEAF_DATA_SIZE(fs_info) - push_space,
		     BTRFS_LEAF_DATA_OFFSET + leaf_data_end(left),
		     push_space);

	memmove_extent_buffer(right, btrfs_item_nr_offset(push_items),
			      btrfs_item_nr_offset(0),
			      right_nritems * sizeof(struct btrfs_item));

	/* copy the items from left to right */
	copy_extent_buffer(right, left, btrfs_item_nr_offset(0),
		   btrfs_item_nr_offset(left_nritems - push_items),
		   push_items * sizeof(struct btrfs_item));

	/* update the item pointers */
	btrfs_init_map_token(&token, right);
	right_nritems += push_items;
	btrfs_set_header_nritems(right, right_nritems);
	push_space = BTRFS_LEAF_DATA_SIZE(fs_info);
	for (i = 0; i < right_nritems; i++) {
		item = btrfs_item_nr(i);
		push_space -= btrfs_token_item_size(&token, item);
		btrfs_set_token_item_offset(&token, item, push_space);
	}

	left_nritems -= push_items;
	btrfs_set_header_nritems(left, left_nritems);

	if (left_nritems)
		btrfs_mark_buffer_dirty(left);
	else
		btrfs_clean_tree_block(left);

	btrfs_mark_buffer_dirty(right);

	btrfs_item_key(right, &disk_key, 0);
	btrfs_set_node_key(upper, &disk_key, slot + 1);
	btrfs_mark_buffer_dirty(upper);

	/* then fixup the leaf pointer in the path */
	if (path->slots[0] >= left_nritems) {
		path->slots[0] -= left_nritems;
		if (btrfs_header_nritems(path->nodes[0]) == 0)
			btrfs_clean_tree_block(path->nodes[0]);
		btrfs_tree_unlock(path->nodes[0]);
		free_extent_buffer(path->nodes[0]);
		path->nodes[0] = right;
		path->slots[1] += 1;
	} else {
		btrfs_tree_unlock(right);
		free_extent_buffer(right);
	}
	return 0;

out_unlock:
	btrfs_tree_unlock(right);
	free_extent_buffer(right);
	return 1;
}