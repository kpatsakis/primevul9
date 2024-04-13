static noinline void copy_for_split(struct btrfs_trans_handle *trans,
				    struct btrfs_path *path,
				    struct extent_buffer *l,
				    struct extent_buffer *right,
				    int slot, int mid, int nritems)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	int data_copy_size;
	int rt_data_off;
	int i;
	struct btrfs_disk_key disk_key;
	struct btrfs_map_token token;

	nritems = nritems - mid;
	btrfs_set_header_nritems(right, nritems);
	data_copy_size = btrfs_item_end_nr(l, mid) - leaf_data_end(l);

	copy_extent_buffer(right, l, btrfs_item_nr_offset(0),
			   btrfs_item_nr_offset(mid),
			   nritems * sizeof(struct btrfs_item));

	copy_extent_buffer(right, l,
		     BTRFS_LEAF_DATA_OFFSET + BTRFS_LEAF_DATA_SIZE(fs_info) -
		     data_copy_size, BTRFS_LEAF_DATA_OFFSET +
		     leaf_data_end(l), data_copy_size);

	rt_data_off = BTRFS_LEAF_DATA_SIZE(fs_info) - btrfs_item_end_nr(l, mid);

	btrfs_init_map_token(&token, right);
	for (i = 0; i < nritems; i++) {
		struct btrfs_item *item = btrfs_item_nr(i);
		u32 ioff;

		ioff = btrfs_token_item_offset(&token, item);
		btrfs_set_token_item_offset(&token, item, ioff + rt_data_off);
	}

	btrfs_set_header_nritems(l, mid);
	btrfs_item_key(right, &disk_key, 0);
	insert_ptr(trans, path, &disk_key, right->start, path->slots[1] + 1, 1);

	btrfs_mark_buffer_dirty(right);
	btrfs_mark_buffer_dirty(l);
	BUG_ON(path->slots[0] != slot);

	if (mid <= slot) {
		btrfs_tree_unlock(path->nodes[0]);
		free_extent_buffer(path->nodes[0]);
		path->nodes[0] = right;
		path->slots[0] -= mid;
		path->slots[1] += 1;
	} else {
		btrfs_tree_unlock(right);
		free_extent_buffer(right);
	}

	BUG_ON(path->slots[0] < 0);
}