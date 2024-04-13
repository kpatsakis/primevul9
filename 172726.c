static noinline_for_stack struct scrub_ctx *scrub_setup_ctx(
		struct btrfs_fs_info *fs_info, int is_dev_replace)
{
	struct scrub_ctx *sctx;
	int		i;

	sctx = kzalloc(sizeof(*sctx), GFP_KERNEL);
	if (!sctx)
		goto nomem;
	refcount_set(&sctx->refs, 1);
	sctx->is_dev_replace = is_dev_replace;
	sctx->pages_per_rd_bio = SCRUB_PAGES_PER_RD_BIO;
	sctx->curr = -1;
	sctx->fs_info = fs_info;
	for (i = 0; i < SCRUB_BIOS_PER_SCTX; ++i) {
		struct scrub_bio *sbio;

		sbio = kzalloc(sizeof(*sbio), GFP_KERNEL);
		if (!sbio)
			goto nomem;
		sctx->bios[i] = sbio;

		sbio->index = i;
		sbio->sctx = sctx;
		sbio->page_count = 0;
		btrfs_init_work(&sbio->work, btrfs_scrub_helper,
				scrub_bio_end_io_worker, NULL, NULL);

		if (i != SCRUB_BIOS_PER_SCTX - 1)
			sctx->bios[i]->next_free = i + 1;
		else
			sctx->bios[i]->next_free = -1;
	}
	sctx->first_free = 0;
	atomic_set(&sctx->bios_in_flight, 0);
	atomic_set(&sctx->workers_pending, 0);
	atomic_set(&sctx->cancel_req, 0);
	sctx->csum_size = btrfs_super_csum_size(fs_info->super_copy);
	INIT_LIST_HEAD(&sctx->csum_list);

	spin_lock_init(&sctx->list_lock);
	spin_lock_init(&sctx->stat_lock);
	init_waitqueue_head(&sctx->list_wait);

	WARN_ON(sctx->wr_curr_bio != NULL);
	mutex_init(&sctx->wr_lock);
	sctx->wr_curr_bio = NULL;
	if (is_dev_replace) {
		WARN_ON(!fs_info->dev_replace.tgtdev);
		sctx->pages_per_wr_bio = SCRUB_PAGES_PER_WR_BIO;
		sctx->wr_tgtdev = fs_info->dev_replace.tgtdev;
		sctx->flush_all_writes = false;
	}

	return sctx;

nomem:
	scrub_free_ctx(sctx);
	return ERR_PTR(-ENOMEM);
}