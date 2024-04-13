static void scrub_wr_bio_end_io_worker(struct btrfs_work *work)
{
	struct scrub_bio *sbio = container_of(work, struct scrub_bio, work);
	struct scrub_ctx *sctx = sbio->sctx;
	int i;

	WARN_ON(sbio->page_count > SCRUB_PAGES_PER_WR_BIO);
	if (sbio->status) {
		struct btrfs_dev_replace *dev_replace =
			&sbio->sctx->fs_info->dev_replace;

		for (i = 0; i < sbio->page_count; i++) {
			struct scrub_page *spage = sbio->pagev[i];

			spage->io_error = 1;
			atomic64_inc(&dev_replace->num_write_errors);
		}
	}

	for (i = 0; i < sbio->page_count; i++)
		scrub_page_put(sbio->pagev[i]);

	bio_put(sbio->bio);
	kfree(sbio);
	scrub_pending_bio_dec(sctx);
}