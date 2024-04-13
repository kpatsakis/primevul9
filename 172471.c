static long _btrfs_ioctl_set_received_subvol(struct file *file,
					    struct btrfs_ioctl_received_subvol_args *sa)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_root_item *root_item = &root->root_item;
	struct btrfs_trans_handle *trans;
	struct timespec64 ct = current_time(inode);
	int ret = 0;
	int received_uuid_changed;

	if (!inode_owner_or_capable(inode))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret < 0)
		return ret;

	down_write(&fs_info->subvol_sem);

	if (btrfs_ino(BTRFS_I(inode)) != BTRFS_FIRST_FREE_OBJECTID) {
		ret = -EINVAL;
		goto out;
	}

	if (btrfs_root_readonly(root)) {
		ret = -EROFS;
		goto out;
	}

	/*
	 * 1 - root item
	 * 2 - uuid items (received uuid + subvol uuid)
	 */
	trans = btrfs_start_transaction(root, 3);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		trans = NULL;
		goto out;
	}

	sa->rtransid = trans->transid;
	sa->rtime.sec = ct.tv_sec;
	sa->rtime.nsec = ct.tv_nsec;

	received_uuid_changed = memcmp(root_item->received_uuid, sa->uuid,
				       BTRFS_UUID_SIZE);
	if (received_uuid_changed &&
	    !btrfs_is_empty_uuid(root_item->received_uuid)) {
		ret = btrfs_uuid_tree_remove(trans, root_item->received_uuid,
					  BTRFS_UUID_KEY_RECEIVED_SUBVOL,
					  root->root_key.objectid);
		if (ret && ret != -ENOENT) {
		        btrfs_abort_transaction(trans, ret);
		        btrfs_end_transaction(trans);
		        goto out;
		}
	}
	memcpy(root_item->received_uuid, sa->uuid, BTRFS_UUID_SIZE);
	btrfs_set_root_stransid(root_item, sa->stransid);
	btrfs_set_root_rtransid(root_item, sa->rtransid);
	btrfs_set_stack_timespec_sec(&root_item->stime, sa->stime.sec);
	btrfs_set_stack_timespec_nsec(&root_item->stime, sa->stime.nsec);
	btrfs_set_stack_timespec_sec(&root_item->rtime, sa->rtime.sec);
	btrfs_set_stack_timespec_nsec(&root_item->rtime, sa->rtime.nsec);

	ret = btrfs_update_root(trans, fs_info->tree_root,
				&root->root_key, &root->root_item);
	if (ret < 0) {
		btrfs_end_transaction(trans);
		goto out;
	}
	if (received_uuid_changed && !btrfs_is_empty_uuid(sa->uuid)) {
		ret = btrfs_uuid_tree_add(trans, sa->uuid,
					  BTRFS_UUID_KEY_RECEIVED_SUBVOL,
					  root->root_key.objectid);
		if (ret < 0 && ret != -EEXIST) {
			btrfs_abort_transaction(trans, ret);
			btrfs_end_transaction(trans);
			goto out;
		}
	}
	ret = btrfs_commit_transaction(trans);
out:
	up_write(&fs_info->subvol_sem);
	mnt_drop_write_file(file);
	return ret;
}