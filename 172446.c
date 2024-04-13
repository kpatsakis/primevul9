static noinline struct btrfs_fs_devices *find_fsid(
		const u8 *fsid, const u8 *metadata_fsid)
{
	struct btrfs_fs_devices *fs_devices;

	ASSERT(fsid);

	if (metadata_fsid) {
		/*
		 * Handle scanned device having completed its fsid change but
		 * belonging to a fs_devices that was created by first scanning
		 * a device which didn't have its fsid/metadata_uuid changed
		 * at all and the CHANGING_FSID_V2 flag set.
		 */
		list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
			if (fs_devices->fsid_change &&
			    memcmp(metadata_fsid, fs_devices->fsid,
				   BTRFS_FSID_SIZE) == 0 &&
			    memcmp(fs_devices->fsid, fs_devices->metadata_uuid,
				   BTRFS_FSID_SIZE) == 0) {
				return fs_devices;
			}
		}
		/*
		 * Handle scanned device having completed its fsid change but
		 * belonging to a fs_devices that was created by a device that
		 * has an outdated pair of fsid/metadata_uuid and
		 * CHANGING_FSID_V2 flag set.
		 */
		list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
			if (fs_devices->fsid_change &&
			    memcmp(fs_devices->metadata_uuid,
				   fs_devices->fsid, BTRFS_FSID_SIZE) != 0 &&
			    memcmp(metadata_fsid, fs_devices->metadata_uuid,
				   BTRFS_FSID_SIZE) == 0) {
				return fs_devices;
			}
		}
	}

	/* Handle non-split brain cases */
	list_for_each_entry(fs_devices, &fs_uuids, fs_list) {
		if (metadata_fsid) {
			if (memcmp(fsid, fs_devices->fsid, BTRFS_FSID_SIZE) == 0
			    && memcmp(metadata_fsid, fs_devices->metadata_uuid,
				      BTRFS_FSID_SIZE) == 0)
				return fs_devices;
		} else {
			if (memcmp(fsid, fs_devices->fsid, BTRFS_FSID_SIZE) == 0)
				return fs_devices;
		}
	}
	return NULL;
}