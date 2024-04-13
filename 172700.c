static void scrub_parity_check_and_repair(struct scrub_parity *sparity)
{
	struct scrub_ctx *sctx = sparity->sctx;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	struct bio *bio;
	struct btrfs_raid_bio *rbio;
	struct btrfs_bio *bbio = NULL;
	u64 length;
	int ret;

	if (!bitmap_andnot(sparity->dbitmap, sparity->dbitmap, sparity->ebitmap,
			   sparity->nsectors))
		goto out;

	length = sparity->logic_end - sparity->logic_start;

	btrfs_bio_counter_inc_blocked(fs_info);
	ret = btrfs_map_sblock(fs_info, BTRFS_MAP_WRITE, sparity->logic_start,
			       &length, &bbio);
	if (ret || !bbio || !bbio->raid_map)
		goto bbio_out;

	bio = btrfs_io_bio_alloc(0);
	bio->bi_iter.bi_sector = sparity->logic_start >> 9;
	bio->bi_private = sparity;
	bio->bi_end_io = scrub_parity_bio_endio;

	rbio = raid56_parity_alloc_scrub_rbio(fs_info, bio, bbio,
					      length, sparity->scrub_dev,
					      sparity->dbitmap,
					      sparity->nsectors);
	if (!rbio)
		goto rbio_out;

	scrub_pending_bio_inc(sctx);
	raid56_parity_submit_scrub_rbio(rbio);
	return;

rbio_out:
	bio_put(bio);
bbio_out:
	btrfs_bio_counter_dec(fs_info);
	btrfs_put_bbio(bbio);
	bitmap_or(sparity->ebitmap, sparity->ebitmap, sparity->dbitmap,
		  sparity->nsectors);
	spin_lock(&sctx->stat_lock);
	sctx->stat.malloc_errors++;
	spin_unlock(&sctx->stat_lock);
out:
	scrub_free_parity(sparity);
}