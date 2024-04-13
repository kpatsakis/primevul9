static long btrfs_ioctl_fs_info(struct btrfs_fs_info *fs_info,
				void __user *arg)
{
	struct btrfs_ioctl_fs_info_args *fi_args;
	struct btrfs_device *device;
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	int ret = 0;

	fi_args = kzalloc(sizeof(*fi_args), GFP_KERNEL);
	if (!fi_args)
		return -ENOMEM;

	rcu_read_lock();
	fi_args->num_devices = fs_devices->num_devices;

	list_for_each_entry_rcu(device, &fs_devices->devices, dev_list) {
		if (device->devid > fi_args->max_id)
			fi_args->max_id = device->devid;
	}
	rcu_read_unlock();

	memcpy(&fi_args->fsid, fs_devices->fsid, sizeof(fi_args->fsid));
	fi_args->nodesize = fs_info->nodesize;
	fi_args->sectorsize = fs_info->sectorsize;
	fi_args->clone_alignment = fs_info->sectorsize;

	if (copy_to_user(arg, fi_args, sizeof(*fi_args)))
		ret = -EFAULT;

	kfree(fi_args);
	return ret;
}