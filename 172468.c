static int lock_full_stripe(struct btrfs_fs_info *fs_info, u64 bytenr,
			    bool *locked_ret)
{
	struct btrfs_block_group_cache *bg_cache;
	struct btrfs_full_stripe_locks_tree *locks_root;
	struct full_stripe_lock *existing;
	u64 fstripe_start;
	int ret = 0;

	*locked_ret = false;
	bg_cache = btrfs_lookup_block_group(fs_info, bytenr);
	if (!bg_cache) {
		ASSERT(0);
		return -ENOENT;
	}

	/* Profiles not based on parity don't need full stripe lock */
	if (!(bg_cache->flags & BTRFS_BLOCK_GROUP_RAID56_MASK))
		goto out;
	locks_root = &bg_cache->full_stripe_locks_root;

	fstripe_start = get_full_stripe_logical(bg_cache, bytenr);

	/* Now insert the full stripe lock */
	mutex_lock(&locks_root->lock);
	existing = insert_full_stripe_lock(locks_root, fstripe_start);
	mutex_unlock(&locks_root->lock);
	if (IS_ERR(existing)) {
		ret = PTR_ERR(existing);
		goto out;
	}
	mutex_lock(&existing->mutex);
	*locked_ret = true;
out:
	btrfs_put_block_group(bg_cache);
	return ret;
}