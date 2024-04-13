static void scrub_missing_raid56_worker(struct btrfs_work *work)
{
	struct scrub_block *sblock = container_of(work, struct scrub_block, work);
	struct scrub_ctx *sctx = sblock->sctx;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	u64 logical;
	struct btrfs_device *dev;

	logical = sblock->pagev[0]->logical;
	dev = sblock->pagev[0]->dev;

	if (sblock->no_io_error_seen)
		scrub_recheck_block_checksum(sblock);

	if (!sblock->no_io_error_seen) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.read_errors++;
		spin_unlock(&sctx->stat_lock);
		btrfs_err_rl_in_rcu(fs_info,
			"IO error rebuilding logical %llu for dev %s",
			logical, rcu_str_deref(dev->name));
	} else if (sblock->header_error || sblock->checksum_error) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.uncorrectable_errors++;
		spin_unlock(&sctx->stat_lock);
		btrfs_err_rl_in_rcu(fs_info,
			"failed to rebuild valid logical %llu for dev %s",
			logical, rcu_str_deref(dev->name));
	} else {
		scrub_write_block_to_dev_replace(sblock);
	}

	scrub_block_put(sblock);

	if (sctx->is_dev_replace && sctx->flush_all_writes) {
		mutex_lock(&sctx->wr_lock);
		scrub_wr_submit(sctx);
		mutex_unlock(&sctx->wr_lock);
	}

	scrub_pending_bio_dec(sctx);
}