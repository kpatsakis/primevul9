int btrfs_run_dev_replace(struct btrfs_trans_handle *trans,
			  struct btrfs_fs_info *fs_info)
{
	int ret;
	struct btrfs_root *dev_root = fs_info->dev_root;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct extent_buffer *eb;
	struct btrfs_dev_replace_item *ptr;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;

	down_read(&dev_replace->rwsem);
	if (!dev_replace->is_valid ||
	    !dev_replace->item_needs_writeback) {
		up_read(&dev_replace->rwsem);
		return 0;
	}
	up_read(&dev_replace->rwsem);

	key.objectid = 0;
	key.type = BTRFS_DEV_REPLACE_KEY;
	key.offset = 0;

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto out;
	}
	ret = btrfs_search_slot(trans, dev_root, &key, path, -1, 1);
	if (ret < 0) {
		btrfs_warn(fs_info,
			   "error %d while searching for dev_replace item!",
			   ret);
		goto out;
	}

	if (ret == 0 &&
	    btrfs_item_size_nr(path->nodes[0], path->slots[0]) < sizeof(*ptr)) {
		/*
		 * need to delete old one and insert a new one.
		 * Since no attempt is made to recover any old state, if the
		 * dev_replace state is 'running', the data on the target
		 * drive is lost.
		 * It would be possible to recover the state: just make sure
		 * that the beginning of the item is never changed and always
		 * contains all the essential information. Then read this
		 * minimal set of information and use it as a base for the
		 * new state.
		 */
		ret = btrfs_del_item(trans, dev_root, path);
		if (ret != 0) {
			btrfs_warn(fs_info,
				   "delete too small dev_replace item failed %d!",
				   ret);
			goto out;
		}
		ret = 1;
	}

	if (ret == 1) {
		/* need to insert a new item */
		btrfs_release_path(path);
		ret = btrfs_insert_empty_item(trans, dev_root, path,
					      &key, sizeof(*ptr));
		if (ret < 0) {
			btrfs_warn(fs_info,
				   "insert dev_replace item failed %d!", ret);
			goto out;
		}
	}

	eb = path->nodes[0];
	ptr = btrfs_item_ptr(eb, path->slots[0],
			     struct btrfs_dev_replace_item);

	down_write(&dev_replace->rwsem);
	if (dev_replace->srcdev)
		btrfs_set_dev_replace_src_devid(eb, ptr,
			dev_replace->srcdev->devid);
	else
		btrfs_set_dev_replace_src_devid(eb, ptr, (u64)-1);
	btrfs_set_dev_replace_cont_reading_from_srcdev_mode(eb, ptr,
		dev_replace->cont_reading_from_srcdev_mode);
	btrfs_set_dev_replace_replace_state(eb, ptr,
		dev_replace->replace_state);
	btrfs_set_dev_replace_time_started(eb, ptr, dev_replace->time_started);
	btrfs_set_dev_replace_time_stopped(eb, ptr, dev_replace->time_stopped);
	btrfs_set_dev_replace_num_write_errors(eb, ptr,
		atomic64_read(&dev_replace->num_write_errors));
	btrfs_set_dev_replace_num_uncorrectable_read_errors(eb, ptr,
		atomic64_read(&dev_replace->num_uncorrectable_read_errors));
	dev_replace->cursor_left_last_write_of_item =
		dev_replace->cursor_left;
	btrfs_set_dev_replace_cursor_left(eb, ptr,
		dev_replace->cursor_left_last_write_of_item);
	btrfs_set_dev_replace_cursor_right(eb, ptr,
		dev_replace->cursor_right);
	dev_replace->item_needs_writeback = 0;
	up_write(&dev_replace->rwsem);

	btrfs_mark_buffer_dirty(eb);

out:
	btrfs_free_path(path);

	return ret;
}