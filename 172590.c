static int btrfs_dev_replace_finishing(struct btrfs_fs_info *fs_info,
				       int scrub_ret)
{
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	struct btrfs_device *tgt_device;
	struct btrfs_device *src_device;
	struct btrfs_root *root = fs_info->tree_root;
	u8 uuid_tmp[BTRFS_UUID_SIZE];
	struct btrfs_trans_handle *trans;
	int ret = 0;

	/* don't allow cancel or unmount to disturb the finishing procedure */
	mutex_lock(&dev_replace->lock_finishing_cancel_unmount);

	down_read(&dev_replace->rwsem);
	/* was the operation canceled, or is it finished? */
	if (dev_replace->replace_state !=
	    BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED) {
		up_read(&dev_replace->rwsem);
		mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);
		return 0;
	}

	tgt_device = dev_replace->tgtdev;
	src_device = dev_replace->srcdev;
	up_read(&dev_replace->rwsem);

	/*
	 * flush all outstanding I/O and inode extent mappings before the
	 * copy operation is declared as being finished
	 */
	ret = btrfs_start_delalloc_roots(fs_info, -1);
	if (ret) {
		mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);
		return ret;
	}
	btrfs_wait_ordered_roots(fs_info, U64_MAX, 0, (u64)-1);

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);
		return PTR_ERR(trans);
	}
	ret = btrfs_commit_transaction(trans);
	WARN_ON(ret);

	/* keep away write_all_supers() during the finishing procedure */
	mutex_lock(&fs_info->fs_devices->device_list_mutex);
	mutex_lock(&fs_info->chunk_mutex);
	down_write(&dev_replace->rwsem);
	dev_replace->replace_state =
		scrub_ret ? BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED
			  : BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED;
	dev_replace->tgtdev = NULL;
	dev_replace->srcdev = NULL;
	dev_replace->time_stopped = ktime_get_real_seconds();
	dev_replace->item_needs_writeback = 1;

	/* replace old device with new one in mapping tree */
	if (!scrub_ret) {
		btrfs_dev_replace_update_device_in_mapping_tree(fs_info,
								src_device,
								tgt_device);
	} else {
		if (scrub_ret != -ECANCELED)
			btrfs_err_in_rcu(fs_info,
				 "btrfs_scrub_dev(%s, %llu, %s) failed %d",
				 btrfs_dev_name(src_device),
				 src_device->devid,
				 rcu_str_deref(tgt_device->name), scrub_ret);
		up_write(&dev_replace->rwsem);
		mutex_unlock(&fs_info->chunk_mutex);
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		btrfs_rm_dev_replace_blocked(fs_info);
		if (tgt_device)
			btrfs_destroy_dev_replace_tgtdev(tgt_device);
		btrfs_rm_dev_replace_unblocked(fs_info);
		mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);

		return scrub_ret;
	}

	btrfs_info_in_rcu(fs_info,
			  "dev_replace from %s (devid %llu) to %s finished",
			  btrfs_dev_name(src_device),
			  src_device->devid,
			  rcu_str_deref(tgt_device->name));
	clear_bit(BTRFS_DEV_STATE_REPLACE_TGT, &tgt_device->dev_state);
	tgt_device->devid = src_device->devid;
	src_device->devid = BTRFS_DEV_REPLACE_DEVID;
	memcpy(uuid_tmp, tgt_device->uuid, sizeof(uuid_tmp));
	memcpy(tgt_device->uuid, src_device->uuid, sizeof(tgt_device->uuid));
	memcpy(src_device->uuid, uuid_tmp, sizeof(src_device->uuid));
	btrfs_device_set_total_bytes(tgt_device, src_device->total_bytes);
	btrfs_device_set_disk_total_bytes(tgt_device,
					  src_device->disk_total_bytes);
	btrfs_device_set_bytes_used(tgt_device, src_device->bytes_used);
	ASSERT(list_empty(&src_device->resized_list));
	tgt_device->commit_total_bytes = src_device->commit_total_bytes;
	tgt_device->commit_bytes_used = src_device->bytes_used;

	btrfs_assign_next_active_device(src_device, tgt_device);

	list_add(&tgt_device->dev_alloc_list, &fs_info->fs_devices->alloc_list);
	fs_info->fs_devices->rw_devices++;

	up_write(&dev_replace->rwsem);
	btrfs_rm_dev_replace_blocked(fs_info);

	btrfs_rm_dev_replace_remove_srcdev(src_device);

	btrfs_rm_dev_replace_unblocked(fs_info);

	/*
	 * Increment dev_stats_ccnt so that btrfs_run_dev_stats() will
	 * update on-disk dev stats value during commit transaction
	 */
	atomic_inc(&tgt_device->dev_stats_ccnt);

	/*
	 * this is again a consistent state where no dev_replace procedure
	 * is running, the target device is part of the filesystem, the
	 * source device is not part of the filesystem anymore and its 1st
	 * superblock is scratched out so that it is no longer marked to
	 * belong to this filesystem.
	 */
	mutex_unlock(&fs_info->chunk_mutex);
	mutex_unlock(&fs_info->fs_devices->device_list_mutex);

	/* replace the sysfs entry */
	btrfs_sysfs_rm_device_link(fs_info->fs_devices, src_device);
	btrfs_rm_dev_replace_free_srcdev(fs_info, src_device);

	/* write back the superblocks */
	trans = btrfs_start_transaction(root, 0);
	if (!IS_ERR(trans))
		btrfs_commit_transaction(trans);

	mutex_unlock(&dev_replace->lock_finishing_cancel_unmount);

	return 0;
}