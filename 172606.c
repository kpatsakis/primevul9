static long btrfs_ioctl_space_info(struct btrfs_fs_info *fs_info,
				   void __user *arg)
{
	struct btrfs_ioctl_space_args space_args;
	struct btrfs_ioctl_space_info space;
	struct btrfs_ioctl_space_info *dest;
	struct btrfs_ioctl_space_info *dest_orig;
	struct btrfs_ioctl_space_info __user *user_dest;
	struct btrfs_space_info *info;
	static const u64 types[] = {
		BTRFS_BLOCK_GROUP_DATA,
		BTRFS_BLOCK_GROUP_SYSTEM,
		BTRFS_BLOCK_GROUP_METADATA,
		BTRFS_BLOCK_GROUP_DATA | BTRFS_BLOCK_GROUP_METADATA
	};
	int num_types = 4;
	int alloc_size;
	int ret = 0;
	u64 slot_count = 0;
	int i, c;

	if (copy_from_user(&space_args,
			   (struct btrfs_ioctl_space_args __user *)arg,
			   sizeof(space_args)))
		return -EFAULT;

	for (i = 0; i < num_types; i++) {
		struct btrfs_space_info *tmp;

		info = NULL;
		rcu_read_lock();
		list_for_each_entry_rcu(tmp, &fs_info->space_info,
					list) {
			if (tmp->flags == types[i]) {
				info = tmp;
				break;
			}
		}
		rcu_read_unlock();

		if (!info)
			continue;

		down_read(&info->groups_sem);
		for (c = 0; c < BTRFS_NR_RAID_TYPES; c++) {
			if (!list_empty(&info->block_groups[c]))
				slot_count++;
		}
		up_read(&info->groups_sem);
	}

	/*
	 * Global block reserve, exported as a space_info
	 */
	slot_count++;

	/* space_slots == 0 means they are asking for a count */
	if (space_args.space_slots == 0) {
		space_args.total_spaces = slot_count;
		goto out;
	}

	slot_count = min_t(u64, space_args.space_slots, slot_count);

	alloc_size = sizeof(*dest) * slot_count;

	/* we generally have at most 6 or so space infos, one for each raid
	 * level.  So, a whole page should be more than enough for everyone
	 */
	if (alloc_size > PAGE_SIZE)
		return -ENOMEM;

	space_args.total_spaces = 0;
	dest = kmalloc(alloc_size, GFP_KERNEL);
	if (!dest)
		return -ENOMEM;
	dest_orig = dest;

	/* now we have a buffer to copy into */
	for (i = 0; i < num_types; i++) {
		struct btrfs_space_info *tmp;

		if (!slot_count)
			break;

		info = NULL;
		rcu_read_lock();
		list_for_each_entry_rcu(tmp, &fs_info->space_info,
					list) {
			if (tmp->flags == types[i]) {
				info = tmp;
				break;
			}
		}
		rcu_read_unlock();

		if (!info)
			continue;
		down_read(&info->groups_sem);
		for (c = 0; c < BTRFS_NR_RAID_TYPES; c++) {
			if (!list_empty(&info->block_groups[c])) {
				get_block_group_info(&info->block_groups[c],
						     &space);
				memcpy(dest, &space, sizeof(space));
				dest++;
				space_args.total_spaces++;
				slot_count--;
			}
			if (!slot_count)
				break;
		}
		up_read(&info->groups_sem);
	}

	/*
	 * Add global block reserve
	 */
	if (slot_count) {
		struct btrfs_block_rsv *block_rsv = &fs_info->global_block_rsv;

		spin_lock(&block_rsv->lock);
		space.total_bytes = block_rsv->size;
		space.used_bytes = block_rsv->size - block_rsv->reserved;
		spin_unlock(&block_rsv->lock);
		space.flags = BTRFS_SPACE_INFO_GLOBAL_RSV;
		memcpy(dest, &space, sizeof(space));
		space_args.total_spaces++;
	}

	user_dest = (struct btrfs_ioctl_space_info __user *)
		(arg + sizeof(struct btrfs_ioctl_space_args));

	if (copy_to_user(user_dest, dest_orig, alloc_size))
		ret = -EFAULT;

	kfree(dest_orig);
out:
	if (ret == 0 && copy_to_user(arg, &space_args, sizeof(space_args)))
		ret = -EFAULT;

	return ret;
}