void btrfs_set_item_key_safe(struct btrfs_fs_info *fs_info,
			     struct btrfs_path *path,
			     const struct btrfs_key *new_key)
{
	struct btrfs_disk_key disk_key;
	struct extent_buffer *eb;
	int slot;

	eb = path->nodes[0];
	slot = path->slots[0];
	if (slot > 0) {
		btrfs_item_key(eb, &disk_key, slot - 1);
		if (unlikely(comp_keys(&disk_key, new_key) >= 0)) {
			btrfs_crit(fs_info,
		"slot %u key (%llu %u %llu) new key (%llu %u %llu)",
				   slot, btrfs_disk_key_objectid(&disk_key),
				   btrfs_disk_key_type(&disk_key),
				   btrfs_disk_key_offset(&disk_key),
				   new_key->objectid, new_key->type,
				   new_key->offset);
			btrfs_print_leaf(eb);
			BUG();
		}
	}
	if (slot < btrfs_header_nritems(eb) - 1) {
		btrfs_item_key(eb, &disk_key, slot + 1);
		if (unlikely(comp_keys(&disk_key, new_key) <= 0)) {
			btrfs_crit(fs_info,
		"slot %u key (%llu %u %llu) new key (%llu %u %llu)",
				   slot, btrfs_disk_key_objectid(&disk_key),
				   btrfs_disk_key_type(&disk_key),
				   btrfs_disk_key_offset(&disk_key),
				   new_key->objectid, new_key->type,
				   new_key->offset);
			btrfs_print_leaf(eb);
			BUG();
		}
	}

	btrfs_cpu_key_to_disk(&disk_key, new_key);
	btrfs_set_item_key(eb, &disk_key, slot);
	btrfs_mark_buffer_dirty(eb);
	if (slot == 0)
		fixup_low_keys(path, &disk_key, 1);
}