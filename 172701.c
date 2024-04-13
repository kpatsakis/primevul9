long btrfs_ioctl(struct file *file, unsigned int
		cmd, unsigned long arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_root *root = BTRFS_I(inode)->root;
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case FS_IOC_GETFLAGS:
		return btrfs_ioctl_getflags(file, argp);
	case FS_IOC_SETFLAGS:
		return btrfs_ioctl_setflags(file, argp);
	case FS_IOC_GETVERSION:
		return btrfs_ioctl_getversion(file, argp);
	case FITRIM:
		return btrfs_ioctl_fitrim(file, argp);
	case BTRFS_IOC_SNAP_CREATE:
		return btrfs_ioctl_snap_create(file, argp, 0);
	case BTRFS_IOC_SNAP_CREATE_V2:
		return btrfs_ioctl_snap_create_v2(file, argp, 0);
	case BTRFS_IOC_SUBVOL_CREATE:
		return btrfs_ioctl_snap_create(file, argp, 1);
	case BTRFS_IOC_SUBVOL_CREATE_V2:
		return btrfs_ioctl_snap_create_v2(file, argp, 1);
	case BTRFS_IOC_SNAP_DESTROY:
		return btrfs_ioctl_snap_destroy(file, argp);
	case BTRFS_IOC_SUBVOL_GETFLAGS:
		return btrfs_ioctl_subvol_getflags(file, argp);
	case BTRFS_IOC_SUBVOL_SETFLAGS:
		return btrfs_ioctl_subvol_setflags(file, argp);
	case BTRFS_IOC_DEFAULT_SUBVOL:
		return btrfs_ioctl_default_subvol(file, argp);
	case BTRFS_IOC_DEFRAG:
		return btrfs_ioctl_defrag(file, NULL);
	case BTRFS_IOC_DEFRAG_RANGE:
		return btrfs_ioctl_defrag(file, argp);
	case BTRFS_IOC_RESIZE:
		return btrfs_ioctl_resize(file, argp);
	case BTRFS_IOC_ADD_DEV:
		return btrfs_ioctl_add_dev(fs_info, argp);
	case BTRFS_IOC_RM_DEV:
		return btrfs_ioctl_rm_dev(file, argp);
	case BTRFS_IOC_RM_DEV_V2:
		return btrfs_ioctl_rm_dev_v2(file, argp);
	case BTRFS_IOC_FS_INFO:
		return btrfs_ioctl_fs_info(fs_info, argp);
	case BTRFS_IOC_DEV_INFO:
		return btrfs_ioctl_dev_info(fs_info, argp);
	case BTRFS_IOC_BALANCE:
		return btrfs_ioctl_balance(file, NULL);
	case BTRFS_IOC_TREE_SEARCH:
		return btrfs_ioctl_tree_search(file, argp);
	case BTRFS_IOC_TREE_SEARCH_V2:
		return btrfs_ioctl_tree_search_v2(file, argp);
	case BTRFS_IOC_INO_LOOKUP:
		return btrfs_ioctl_ino_lookup(file, argp);
	case BTRFS_IOC_INO_PATHS:
		return btrfs_ioctl_ino_to_path(root, argp);
	case BTRFS_IOC_LOGICAL_INO:
		return btrfs_ioctl_logical_to_ino(fs_info, argp, 1);
	case BTRFS_IOC_LOGICAL_INO_V2:
		return btrfs_ioctl_logical_to_ino(fs_info, argp, 2);
	case BTRFS_IOC_SPACE_INFO:
		return btrfs_ioctl_space_info(fs_info, argp);
	case BTRFS_IOC_SYNC: {
		int ret;

		ret = btrfs_start_delalloc_roots(fs_info, -1);
		if (ret)
			return ret;
		ret = btrfs_sync_fs(inode->i_sb, 1);
		/*
		 * The transaction thread may want to do more work,
		 * namely it pokes the cleaner kthread that will start
		 * processing uncleaned subvols.
		 */
		wake_up_process(fs_info->transaction_kthread);
		return ret;
	}
	case BTRFS_IOC_START_SYNC:
		return btrfs_ioctl_start_sync(root, argp);
	case BTRFS_IOC_WAIT_SYNC:
		return btrfs_ioctl_wait_sync(fs_info, argp);
	case BTRFS_IOC_SCRUB:
		return btrfs_ioctl_scrub(file, argp);
	case BTRFS_IOC_SCRUB_CANCEL:
		return btrfs_ioctl_scrub_cancel(fs_info);
	case BTRFS_IOC_SCRUB_PROGRESS:
		return btrfs_ioctl_scrub_progress(fs_info, argp);
	case BTRFS_IOC_BALANCE_V2:
		return btrfs_ioctl_balance(file, argp);
	case BTRFS_IOC_BALANCE_CTL:
		return btrfs_ioctl_balance_ctl(fs_info, arg);
	case BTRFS_IOC_BALANCE_PROGRESS:
		return btrfs_ioctl_balance_progress(fs_info, argp);
	case BTRFS_IOC_SET_RECEIVED_SUBVOL:
		return btrfs_ioctl_set_received_subvol(file, argp);
#ifdef CONFIG_64BIT
	case BTRFS_IOC_SET_RECEIVED_SUBVOL_32:
		return btrfs_ioctl_set_received_subvol_32(file, argp);
#endif
	case BTRFS_IOC_SEND:
		return _btrfs_ioctl_send(file, argp, false);
#if defined(CONFIG_64BIT) && defined(CONFIG_COMPAT)
	case BTRFS_IOC_SEND_32:
		return _btrfs_ioctl_send(file, argp, true);
#endif
	case BTRFS_IOC_GET_DEV_STATS:
		return btrfs_ioctl_get_dev_stats(fs_info, argp);
	case BTRFS_IOC_QUOTA_CTL:
		return btrfs_ioctl_quota_ctl(file, argp);
	case BTRFS_IOC_QGROUP_ASSIGN:
		return btrfs_ioctl_qgroup_assign(file, argp);
	case BTRFS_IOC_QGROUP_CREATE:
		return btrfs_ioctl_qgroup_create(file, argp);
	case BTRFS_IOC_QGROUP_LIMIT:
		return btrfs_ioctl_qgroup_limit(file, argp);
	case BTRFS_IOC_QUOTA_RESCAN:
		return btrfs_ioctl_quota_rescan(file, argp);
	case BTRFS_IOC_QUOTA_RESCAN_STATUS:
		return btrfs_ioctl_quota_rescan_status(file, argp);
	case BTRFS_IOC_QUOTA_RESCAN_WAIT:
		return btrfs_ioctl_quota_rescan_wait(file, argp);
	case BTRFS_IOC_DEV_REPLACE:
		return btrfs_ioctl_dev_replace(fs_info, argp);
	case BTRFS_IOC_GET_FSLABEL:
		return btrfs_ioctl_get_fslabel(file, argp);
	case BTRFS_IOC_SET_FSLABEL:
		return btrfs_ioctl_set_fslabel(file, argp);
	case BTRFS_IOC_GET_SUPPORTED_FEATURES:
		return btrfs_ioctl_get_supported_features(argp);
	case BTRFS_IOC_GET_FEATURES:
		return btrfs_ioctl_get_features(file, argp);
	case BTRFS_IOC_SET_FEATURES:
		return btrfs_ioctl_set_features(file, argp);
	case FS_IOC_FSGETXATTR:
		return btrfs_ioctl_fsgetxattr(file, argp);
	case FS_IOC_FSSETXATTR:
		return btrfs_ioctl_fssetxattr(file, argp);
	case BTRFS_IOC_GET_SUBVOL_INFO:
		return btrfs_ioctl_get_subvol_info(file, argp);
	case BTRFS_IOC_GET_SUBVOL_ROOTREF:
		return btrfs_ioctl_get_subvol_rootref(file, argp);
	case BTRFS_IOC_INO_LOOKUP_USER:
		return btrfs_ioctl_ino_lookup_user(file, argp);
	}

	return -ENOTTY;
}