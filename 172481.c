static int btrfs_ioctl_setflags(struct file *file, void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	struct btrfs_inode *binode = BTRFS_I(inode);
	struct btrfs_root *root = binode->root;
	struct btrfs_trans_handle *trans;
	unsigned int fsflags, old_fsflags;
	int ret;
	u64 old_flags;
	unsigned int old_i_flags;
	umode_t mode;

	if (!inode_owner_or_capable(inode))
		return -EPERM;

	if (btrfs_root_readonly(root))
		return -EROFS;

	if (copy_from_user(&fsflags, arg, sizeof(fsflags)))
		return -EFAULT;

	ret = check_fsflags(fsflags);
	if (ret)
		return ret;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	inode_lock(inode);

	old_flags = binode->flags;
	old_i_flags = inode->i_flags;
	mode = inode->i_mode;

	fsflags = btrfs_mask_fsflags_for_type(inode, fsflags);
	old_fsflags = btrfs_inode_flags_to_fsflags(binode->flags);
	if ((fsflags ^ old_fsflags) & (FS_APPEND_FL | FS_IMMUTABLE_FL)) {
		if (!capable(CAP_LINUX_IMMUTABLE)) {
			ret = -EPERM;
			goto out_unlock;
		}
	}

	if (fsflags & FS_SYNC_FL)
		binode->flags |= BTRFS_INODE_SYNC;
	else
		binode->flags &= ~BTRFS_INODE_SYNC;
	if (fsflags & FS_IMMUTABLE_FL)
		binode->flags |= BTRFS_INODE_IMMUTABLE;
	else
		binode->flags &= ~BTRFS_INODE_IMMUTABLE;
	if (fsflags & FS_APPEND_FL)
		binode->flags |= BTRFS_INODE_APPEND;
	else
		binode->flags &= ~BTRFS_INODE_APPEND;
	if (fsflags & FS_NODUMP_FL)
		binode->flags |= BTRFS_INODE_NODUMP;
	else
		binode->flags &= ~BTRFS_INODE_NODUMP;
	if (fsflags & FS_NOATIME_FL)
		binode->flags |= BTRFS_INODE_NOATIME;
	else
		binode->flags &= ~BTRFS_INODE_NOATIME;
	if (fsflags & FS_DIRSYNC_FL)
		binode->flags |= BTRFS_INODE_DIRSYNC;
	else
		binode->flags &= ~BTRFS_INODE_DIRSYNC;
	if (fsflags & FS_NOCOW_FL) {
		if (S_ISREG(mode)) {
			/*
			 * It's safe to turn csums off here, no extents exist.
			 * Otherwise we want the flag to reflect the real COW
			 * status of the file and will not set it.
			 */
			if (inode->i_size == 0)
				binode->flags |= BTRFS_INODE_NODATACOW
					      | BTRFS_INODE_NODATASUM;
		} else {
			binode->flags |= BTRFS_INODE_NODATACOW;
		}
	} else {
		/*
		 * Revert back under same assumptions as above
		 */
		if (S_ISREG(mode)) {
			if (inode->i_size == 0)
				binode->flags &= ~(BTRFS_INODE_NODATACOW
				             | BTRFS_INODE_NODATASUM);
		} else {
			binode->flags &= ~BTRFS_INODE_NODATACOW;
		}
	}

	/*
	 * The COMPRESS flag can only be changed by users, while the NOCOMPRESS
	 * flag may be changed automatically if compression code won't make
	 * things smaller.
	 */
	if (fsflags & FS_NOCOMP_FL) {
		binode->flags &= ~BTRFS_INODE_COMPRESS;
		binode->flags |= BTRFS_INODE_NOCOMPRESS;

		ret = btrfs_set_prop(inode, "btrfs.compression", NULL, 0, 0);
		if (ret && ret != -ENODATA)
			goto out_drop;
	} else if (fsflags & FS_COMPR_FL) {
		const char *comp;

		if (IS_SWAPFILE(inode)) {
			ret = -ETXTBSY;
			goto out_unlock;
		}

		binode->flags |= BTRFS_INODE_COMPRESS;
		binode->flags &= ~BTRFS_INODE_NOCOMPRESS;

		comp = btrfs_compress_type2str(fs_info->compress_type);
		if (!comp || comp[0] == 0)
			comp = btrfs_compress_type2str(BTRFS_COMPRESS_ZLIB);

		ret = btrfs_set_prop(inode, "btrfs.compression",
				     comp, strlen(comp), 0);
		if (ret)
			goto out_drop;

	} else {
		ret = btrfs_set_prop(inode, "btrfs.compression", NULL, 0, 0);
		if (ret && ret != -ENODATA)
			goto out_drop;
		binode->flags &= ~(BTRFS_INODE_COMPRESS | BTRFS_INODE_NOCOMPRESS);
	}

	trans = btrfs_start_transaction(root, 1);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_drop;
	}

	btrfs_sync_inode_flags_to_i_flags(inode);
	inode_inc_iversion(inode);
	inode->i_ctime = current_time(inode);
	ret = btrfs_update_inode(trans, root, inode);

	btrfs_end_transaction(trans);
 out_drop:
	if (ret) {
		binode->flags = old_flags;
		inode->i_flags = old_i_flags;
	}

 out_unlock:
	inode_unlock(inode);
	mnt_drop_write_file(file);
	return ret;
}