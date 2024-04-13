static void fixup_low_keys(struct btrfs_path *path,
			   struct btrfs_disk_key *key, int level)
{
	int i;
	struct extent_buffer *t;
	int ret;

	for (i = level; i < BTRFS_MAX_LEVEL; i++) {
		int tslot = path->slots[i];

		if (!path->nodes[i])
			break;
		t = path->nodes[i];
		ret = tree_mod_log_insert_key(t, tslot, MOD_LOG_KEY_REPLACE,
				GFP_ATOMIC);
		BUG_ON(ret < 0);
		btrfs_set_node_key(t, key, tslot);
		btrfs_mark_buffer_dirty(path->nodes[i]);
		if (tslot != 0)
			break;
	}
}