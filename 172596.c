static long btrfs_ioctl_ino_to_path(struct btrfs_root *root, void __user *arg)
{
	int ret = 0;
	int i;
	u64 rel_ptr;
	int size;
	struct btrfs_ioctl_ino_path_args *ipa = NULL;
	struct inode_fs_paths *ipath = NULL;
	struct btrfs_path *path;

	if (!capable(CAP_DAC_READ_SEARCH))
		return -EPERM;

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto out;
	}

	ipa = memdup_user(arg, sizeof(*ipa));
	if (IS_ERR(ipa)) {
		ret = PTR_ERR(ipa);
		ipa = NULL;
		goto out;
	}

	size = min_t(u32, ipa->size, 4096);
	ipath = init_ipath(size, root, path);
	if (IS_ERR(ipath)) {
		ret = PTR_ERR(ipath);
		ipath = NULL;
		goto out;
	}

	ret = paths_from_inode(ipa->inum, ipath);
	if (ret < 0)
		goto out;

	for (i = 0; i < ipath->fspath->elem_cnt; ++i) {
		rel_ptr = ipath->fspath->val[i] -
			  (u64)(unsigned long)ipath->fspath->val;
		ipath->fspath->val[i] = rel_ptr;
	}

	ret = copy_to_user((void __user *)(unsigned long)ipa->fspath,
			   ipath->fspath, size);
	if (ret) {
		ret = -EFAULT;
		goto out;
	}

out:
	btrfs_free_path(path);
	free_ipath(ipath);
	kfree(ipa);

	return ret;
}