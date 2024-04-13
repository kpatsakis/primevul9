void btrfs_update_commit_device_bytes_used(struct btrfs_transaction *trans)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct extent_map *em;
	struct map_lookup *map;
	struct btrfs_device *dev;
	int i;

	if (list_empty(&trans->pending_chunks))
		return;

	/* In order to kick the device replace finish process */
	mutex_lock(&fs_info->chunk_mutex);
	list_for_each_entry(em, &trans->pending_chunks, list) {
		map = em->map_lookup;

		for (i = 0; i < map->num_stripes; i++) {
			dev = map->stripes[i].dev;
			dev->commit_bytes_used = dev->bytes_used;
		}
	}
	mutex_unlock(&fs_info->chunk_mutex);
}