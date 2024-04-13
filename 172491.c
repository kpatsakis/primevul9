static long btrfs_ioctl_logical_to_ino(struct btrfs_fs_info *fs_info,
					void __user *arg, int version)
{
	int ret = 0;
	int size;
	struct btrfs_ioctl_logical_ino_args *loi;
	struct btrfs_data_container *inodes = NULL;
	struct btrfs_path *path = NULL;
	bool ignore_offset;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	loi = memdup_user(arg, sizeof(*loi));
	if (IS_ERR(loi))
		return PTR_ERR(loi);

	if (version == 1) {
		ignore_offset = false;
		size = min_t(u32, loi->size, SZ_64K);
	} else {
		/* All reserved bits must be 0 for now */
		if (memchr_inv(loi->reserved, 0, sizeof(loi->reserved))) {
			ret = -EINVAL;
			goto out_loi;
		}
		/* Only accept flags we have defined so far */
		if (loi->flags & ~(BTRFS_LOGICAL_INO_ARGS_IGNORE_OFFSET)) {
			ret = -EINVAL;
			goto out_loi;
		}
		ignore_offset = loi->flags & BTRFS_LOGICAL_INO_ARGS_IGNORE_OFFSET;
		size = min_t(u32, loi->size, SZ_16M);
	}

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto out;
	}

	inodes = init_data_container(size);
	if (IS_ERR(inodes)) {
		ret = PTR_ERR(inodes);
		inodes = NULL;
		goto out;
	}

	ret = iterate_inodes_from_logical(loi->logical, fs_info, path,
					  build_ino_list, inodes, ignore_offset);
	if (ret == -EINVAL)
		ret = -ENOENT;
	if (ret < 0)
		goto out;

	ret = copy_to_user((void __user *)(unsigned long)loi->inodes, inodes,
			   size);
	if (ret)
		ret = -EFAULT;

out:
	btrfs_free_path(path);
	kvfree(inodes);
out_loi:
	kfree(loi);

	return ret;
}