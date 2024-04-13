static int unlock_full_stripe(struct btrfs_fs_info *fs_info, u64 bytenr,
			      bool locked)
{
	struct btrfs_block_group_cache *bg_cache;
	struct btrfs_full_stripe_locks_tree *locks_root;
	struct full_stripe_lock *fstripe_lock;
	u64 fstripe_start;
	bool freeit = false;
	int ret = 0;

	/* If we didn't acquire full stripe lock, no need to continue */
	if (!locked)
		return 0;

	bg_cache = btrfs_lookup_block_group(fs_info, bytenr);
	if (!bg_cache) {
		ASSERT(0);
		return -ENOENT;
	}
	if (!(bg_cache->flags & BTRFS_BLOCK_GROUP_RAID56_MASK))
		goto out;

	locks_root = &bg_cache->full_stripe_locks_root;
	fstripe_start = get_full_stripe_logical(bg_cache, bytenr);

	mutex_lock(&locks_root->lock);
	fstripe_lock = search_full_stripe_lock(locks_root, fstripe_start);
	/* Unpaired unlock_full_stripe() detected */
	if (!fstripe_lock) {
		WARN_ON(1);
		ret = -ENOENT;
		mutex_unlock(&locks_root->lock);
		goto out;
	}

	if (fstripe_lock->refs == 0) {
		WARN_ON(1);
		btrfs_warn(fs_info, "full stripe lock at %llu refcount underflow",
			fstripe_lock->logical);
	} else {
		fstripe_lock->refs--;
	}

	if (fstripe_lock->refs == 0) {
		rb_erase(&fstripe_lock->node, &locks_root->root);
		freeit = true;
	}
	mutex_unlock(&locks_root->lock);

	mutex_unlock(&fstripe_lock->mutex);
	if (freeit)
		kfree(fstripe_lock);
out:
	btrfs_put_block_group(bg_cache);
	return ret;
}