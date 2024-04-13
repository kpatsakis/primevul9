static noinline int btrfs_ioctl_snap_create_v2(struct file *file,
					       void __user *arg, int subvol)
{
	struct btrfs_ioctl_vol_args_v2 *vol_args;
	int ret;
	u64 transid = 0;
	u64 *ptr = NULL;
	bool readonly = false;
	struct btrfs_qgroup_inherit *inherit = NULL;

	if (!S_ISDIR(file_inode(file)->i_mode))
		return -ENOTDIR;

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args))
		return PTR_ERR(vol_args);
	vol_args->name[BTRFS_SUBVOL_NAME_MAX] = '\0';

	if (vol_args->flags &
	    ~(BTRFS_SUBVOL_CREATE_ASYNC | BTRFS_SUBVOL_RDONLY |
	      BTRFS_SUBVOL_QGROUP_INHERIT)) {
		ret = -EOPNOTSUPP;
		goto free_args;
	}

	if (vol_args->flags & BTRFS_SUBVOL_CREATE_ASYNC)
		ptr = &transid;
	if (vol_args->flags & BTRFS_SUBVOL_RDONLY)
		readonly = true;
	if (vol_args->flags & BTRFS_SUBVOL_QGROUP_INHERIT) {
		if (vol_args->size > PAGE_SIZE) {
			ret = -EINVAL;
			goto free_args;
		}
		inherit = memdup_user(vol_args->qgroup_inherit, vol_args->size);
		if (IS_ERR(inherit)) {
			ret = PTR_ERR(inherit);
			goto free_args;
		}
	}

	ret = btrfs_ioctl_snap_create_transid(file, vol_args->name,
					      vol_args->fd, subvol, ptr,
					      readonly, inherit);
	if (ret)
		goto free_inherit;

	if (ptr && copy_to_user(arg +
				offsetof(struct btrfs_ioctl_vol_args_v2,
					transid),
				ptr, sizeof(*ptr)))
		ret = -EFAULT;

free_inherit:
	kfree(inherit);
free_args:
	kfree(vol_args);
	return ret;
}