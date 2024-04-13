int btrfs_recover_balance(struct btrfs_fs_info *fs_info)
{
	struct btrfs_balance_control *bctl;
	struct btrfs_balance_item *item;
	struct btrfs_disk_balance_args disk_bargs;
	struct btrfs_path *path;
	struct extent_buffer *leaf;
	struct btrfs_key key;
	int ret;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	key.objectid = BTRFS_BALANCE_OBJECTID;
	key.type = BTRFS_TEMPORARY_ITEM_KEY;
	key.offset = 0;

	ret = btrfs_search_slot(NULL, fs_info->tree_root, &key, path, 0, 0);
	if (ret < 0)
		goto out;
	if (ret > 0) { /* ret = -ENOENT; */
		ret = 0;
		goto out;
	}

	bctl = kzalloc(sizeof(*bctl), GFP_NOFS);
	if (!bctl) {
		ret = -ENOMEM;
		goto out;
	}

	leaf = path->nodes[0];
	item = btrfs_item_ptr(leaf, path->slots[0], struct btrfs_balance_item);

	bctl->flags = btrfs_balance_flags(leaf, item);
	bctl->flags |= BTRFS_BALANCE_RESUME;

	btrfs_balance_data(leaf, item, &disk_bargs);
	btrfs_disk_balance_args_to_cpu(&bctl->data, &disk_bargs);
	btrfs_balance_meta(leaf, item, &disk_bargs);
	btrfs_disk_balance_args_to_cpu(&bctl->meta, &disk_bargs);
	btrfs_balance_sys(leaf, item, &disk_bargs);
	btrfs_disk_balance_args_to_cpu(&bctl->sys, &disk_bargs);

	/*
	 * This should never happen, as the paused balance state is recovered
	 * during mount without any chance of other exclusive ops to collide.
	 *
	 * This gives the exclusive op status to balance and keeps in paused
	 * state until user intervention (cancel or umount). If the ownership
	 * cannot be assigned, show a message but do not fail. The balance
	 * is in a paused state and must have fs_info::balance_ctl properly
	 * set up.
	 */
	if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags))
		btrfs_warn(fs_info,
	"balance: cannot set exclusive op status, resume manually");

	mutex_lock(&fs_info->balance_mutex);
	BUG_ON(fs_info->balance_ctl);
	spin_lock(&fs_info->balance_lock);
	fs_info->balance_ctl = bctl;
	spin_unlock(&fs_info->balance_lock);
	mutex_unlock(&fs_info->balance_mutex);
out:
	btrfs_free_path(path);
	return ret;
}