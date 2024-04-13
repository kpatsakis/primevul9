static void scrub_parity_bio_endio(struct bio *bio)
{
	struct scrub_parity *sparity = (struct scrub_parity *)bio->bi_private;
	struct btrfs_fs_info *fs_info = sparity->sctx->fs_info;

	if (bio->bi_status)
		bitmap_or(sparity->ebitmap, sparity->ebitmap, sparity->dbitmap,
			  sparity->nsectors);

	bio_put(bio);

	btrfs_init_work(&sparity->work, btrfs_scrubparity_helper,
			scrub_parity_bio_endio_worker, NULL, NULL);
	btrfs_queue_work(fs_info->scrub_parity_workers, &sparity->work);
}