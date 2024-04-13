static struct btrfs_fs_devices *find_fsid_changed(
					struct btrfs_super_block *disk_super)
{
	struct btrfs_fs_devices *fs_devices;

	/*
	 * Handles the case where scanned device is part of an fs that had
	 * multiple successful changes of FSID but curently device didn't
	 * observe it. Meaning our fsid will be different than theirs.
	 */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		if (memcmp(fs_devices->metadata_uuid, fs_devices->fsid,
			   BTRFS_FSID_SIZE) != 0 &&
		    memcmp(fs_devices->metadata_uuid, disk_super->metadata_uuid,
			   BTRFS_FSID_SIZE) == 0 &&
		    memcmp(fs_devices->fsid, disk_super->fsid,
			   BTRFS_FSID_SIZE) != 0) {
			return fs_devices;
		}
	}

	return NULL;
}