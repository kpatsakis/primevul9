int btrfs_balance(struct btrfs_fs_info *fs_info,
		  struct btrfs_balance_control *bctl,
		  struct btrfs_ioctl_balance_args *bargs)
{
	u64 meta_target, data_target;
	u64 allowed;
	int mixed = 0;
	int ret;
	u64 num_devices;
	unsigned seq;
	bool reducing_integrity;

	if (btrfs_fs_closing(fs_info) ||
	    atomic_read(&fs_info->balance_pause_req) ||
	    atomic_read(&fs_info->balance_cancel_req)) {
		ret = -EINVAL;
		goto out;
	}

	allowed = btrfs_super_incompat_flags(fs_info->super_copy);
	if (allowed & BTRFS_FEATURE_INCOMPAT_MIXED_GROUPS)
		mixed = 1;

	/*
	 * In case of mixed groups both data and meta should be picked,
	 * and identical options should be given for both of them.
	 */
	allowed = BTRFS_BALANCE_DATA | BTRFS_BALANCE_METADATA;
	if (mixed && (bctl->flags & allowed)) {
		if (!(bctl->flags & BTRFS_BALANCE_DATA) ||
		    !(bctl->flags & BTRFS_BALANCE_METADATA) ||
		    memcmp(&bctl->data, &bctl->meta, sizeof(bctl->data))) {
			btrfs_err(fs_info,
	  "balance: mixed groups data and metadata options must be the same");
			ret = -EINVAL;
			goto out;
		}
	}

	num_devices = btrfs_num_devices(fs_info);

	allowed = BTRFS_AVAIL_ALLOC_BIT_SINGLE | BTRFS_BLOCK_GROUP_DUP;
	if (num_devices > 1)
		allowed |= (BTRFS_BLOCK_GROUP_RAID0 | BTRFS_BLOCK_GROUP_RAID1);
	if (num_devices > 2)
		allowed |= BTRFS_BLOCK_GROUP_RAID5;
	if (num_devices > 3)
		allowed |= (BTRFS_BLOCK_GROUP_RAID10 |
			    BTRFS_BLOCK_GROUP_RAID6);
	if (validate_convert_profile(&bctl->data, allowed)) {
		int index = btrfs_bg_flags_to_raid_index(bctl->data.target);

		btrfs_err(fs_info,
			  "balance: invalid convert data profile %s",
			  get_raid_name(index));
		ret = -EINVAL;
		goto out;
	}
	if (validate_convert_profile(&bctl->meta, allowed)) {
		int index = btrfs_bg_flags_to_raid_index(bctl->meta.target);

		btrfs_err(fs_info,
			  "balance: invalid convert metadata profile %s",
			  get_raid_name(index));
		ret = -EINVAL;
		goto out;
	}
	if (validate_convert_profile(&bctl->sys, allowed)) {
		int index = btrfs_bg_flags_to_raid_index(bctl->sys.target);

		btrfs_err(fs_info,
			  "balance: invalid convert system profile %s",
			  get_raid_name(index));
		ret = -EINVAL;
		goto out;
	}

	/* allow to reduce meta or sys integrity only if force set */
	allowed = BTRFS_BLOCK_GROUP_DUP | BTRFS_BLOCK_GROUP_RAID1 |
			BTRFS_BLOCK_GROUP_RAID10 |
			BTRFS_BLOCK_GROUP_RAID5 |
			BTRFS_BLOCK_GROUP_RAID6;
	do {
		seq = read_seqbegin(&fs_info->profiles_lock);

		if (((bctl->sys.flags & BTRFS_BALANCE_ARGS_CONVERT) &&
		     (fs_info->avail_system_alloc_bits & allowed) &&
		     !(bctl->sys.target & allowed)) ||
		    ((bctl->meta.flags & BTRFS_BALANCE_ARGS_CONVERT) &&
		     (fs_info->avail_metadata_alloc_bits & allowed) &&
		     !(bctl->meta.target & allowed)))
			reducing_integrity = true;
		else
			reducing_integrity = false;

		/* if we're not converting, the target field is uninitialized */
		meta_target = (bctl->meta.flags & BTRFS_BALANCE_ARGS_CONVERT) ?
			bctl->meta.target : fs_info->avail_metadata_alloc_bits;
		data_target = (bctl->data.flags & BTRFS_BALANCE_ARGS_CONVERT) ?
			bctl->data.target : fs_info->avail_data_alloc_bits;
	} while (read_seqretry(&fs_info->profiles_lock, seq));

	if (reducing_integrity) {
		if (bctl->flags & BTRFS_BALANCE_FORCE) {
			btrfs_info(fs_info,
				   "balance: force reducing metadata integrity");
		} else {
			btrfs_err(fs_info,
	  "balance: reduces metadata integrity, use --force if you want this");
			ret = -EINVAL;
			goto out;
		}
	}

	if (btrfs_get_num_tolerated_disk_barrier_failures(meta_target) <
		btrfs_get_num_tolerated_disk_barrier_failures(data_target)) {
		int meta_index = btrfs_bg_flags_to_raid_index(meta_target);
		int data_index = btrfs_bg_flags_to_raid_index(data_target);

		btrfs_warn(fs_info,
	"balance: metadata profile %s has lower redundancy than data profile %s",
			   get_raid_name(meta_index), get_raid_name(data_index));
	}

	ret = insert_balance_item(fs_info, bctl);
	if (ret && ret != -EEXIST)
		goto out;

	if (!(bctl->flags & BTRFS_BALANCE_RESUME)) {
		BUG_ON(ret == -EEXIST);
		BUG_ON(fs_info->balance_ctl);
		spin_lock(&fs_info->balance_lock);
		fs_info->balance_ctl = bctl;
		spin_unlock(&fs_info->balance_lock);
	} else {
		BUG_ON(ret != -EEXIST);
		spin_lock(&fs_info->balance_lock);
		update_balance_args(bctl);
		spin_unlock(&fs_info->balance_lock);
	}

	ASSERT(!test_bit(BTRFS_FS_BALANCE_RUNNING, &fs_info->flags));
	set_bit(BTRFS_FS_BALANCE_RUNNING, &fs_info->flags);
	describe_balance_start_or_resume(fs_info);
	mutex_unlock(&fs_info->balance_mutex);

	ret = __btrfs_balance(fs_info);

	mutex_lock(&fs_info->balance_mutex);
	if (ret == -ECANCELED && atomic_read(&fs_info->balance_pause_req))
		btrfs_info(fs_info, "balance: paused");
	else if (ret == -ECANCELED && atomic_read(&fs_info->balance_cancel_req))
		btrfs_info(fs_info, "balance: canceled");
	else
		btrfs_info(fs_info, "balance: ended with status: %d", ret);

	clear_bit(BTRFS_FS_BALANCE_RUNNING, &fs_info->flags);

	if (bargs) {
		memset(bargs, 0, sizeof(*bargs));
		btrfs_update_ioctl_balance_args(fs_info, bargs);
	}

	if ((ret && ret != -ECANCELED && ret != -ENOSPC) ||
	    balance_need_close(fs_info)) {
		reset_balance_state(fs_info);
		clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);
	}

	wake_up(&fs_info->balance_wait_q);

	return ret;
out:
	if (bctl->flags & BTRFS_BALANCE_RESUME)
		reset_balance_state(fs_info);
	else
		kfree(bctl);
	clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);

	return ret;
}