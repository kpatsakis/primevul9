static int btrfs_add_system_chunk(struct btrfs_fs_info *fs_info,
			   struct btrfs_key *key,
			   struct btrfs_chunk *chunk, int item_size)
{
	struct btrfs_super_block *super_copy = fs_info->super_copy;
	struct btrfs_disk_key disk_key;
	u32 array_size;
	u8 *ptr;

	mutex_lock(&fs_info->chunk_mutex);
	array_size = btrfs_super_sys_array_size(super_copy);
	if (array_size + item_size + sizeof(disk_key)
			> BTRFS_SYSTEM_CHUNK_ARRAY_SIZE) {
		mutex_unlock(&fs_info->chunk_mutex);
		return -EFBIG;
	}

	ptr = super_copy->sys_chunk_array + array_size;
	btrfs_cpu_key_to_disk(&disk_key, key);
	memcpy(ptr, &disk_key, sizeof(disk_key));
	ptr += sizeof(disk_key);
	memcpy(ptr, chunk, item_size);
	item_size += sizeof(disk_key);
	btrfs_set_super_sys_array_size(super_copy, array_size + item_size);
	mutex_unlock(&fs_info->chunk_mutex);

	return 0;
}