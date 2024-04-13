static noinline int setup_leaf_for_split(struct btrfs_trans_handle *trans,
					 struct btrfs_root *root,
					 struct btrfs_path *path, int ins_len)
{
	struct btrfs_key key;
	struct extent_buffer *leaf;
	struct btrfs_file_extent_item *fi;
	u64 extent_len = 0;
	u32 item_size;
	int ret;

	leaf = path->nodes[0];
	btrfs_item_key_to_cpu(leaf, &key, path->slots[0]);

	BUG_ON(key.type != BTRFS_EXTENT_DATA_KEY &&
	       key.type != BTRFS_EXTENT_CSUM_KEY);

	if (btrfs_leaf_free_space(leaf) >= ins_len)
		return 0;

	item_size = btrfs_item_size_nr(leaf, path->slots[0]);
	if (key.type == BTRFS_EXTENT_DATA_KEY) {
		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		extent_len = btrfs_file_extent_num_bytes(leaf, fi);
	}
	btrfs_release_path(path);

	path->keep_locks = 1;
	path->search_for_split = 1;
	ret = btrfs_search_slot(trans, root, &key, path, 0, 1);
	path->search_for_split = 0;
	if (ret > 0)
		ret = -EAGAIN;
	if (ret < 0)
		goto err;

	ret = -EAGAIN;
	leaf = path->nodes[0];
	/* if our item isn't there, return now */
	if (item_size != btrfs_item_size_nr(leaf, path->slots[0]))
		goto err;

	/* the leaf has  changed, it now has room.  return now */
	if (btrfs_leaf_free_space(path->nodes[0]) >= ins_len)
		goto err;

	if (key.type == BTRFS_EXTENT_DATA_KEY) {
		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		if (extent_len != btrfs_file_extent_num_bytes(leaf, fi))
			goto err;
	}

	ret = split_leaf(trans, root, &key, path, ins_len, 1);
	if (ret)
		goto err;

	path->keep_locks = 0;
	btrfs_unlock_up_safe(path, 1);
	return 0;
err:
	path->keep_locks = 0;
	return ret;
}