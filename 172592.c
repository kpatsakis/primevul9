static int scrub_add_page_to_rd_bio(struct scrub_ctx *sctx,
				    struct scrub_page *spage)
{
	struct scrub_block *sblock = spage->sblock;
	struct scrub_bio *sbio;
	int ret;

again:
	/*
	 * grab a fresh bio or wait for one to become available
	 */
	while (sctx->curr == -1) {
		spin_lock(&sctx->list_lock);
		sctx->curr = sctx->first_free;
		if (sctx->curr != -1) {
			sctx->first_free = sctx->bios[sctx->curr]->next_free;
			sctx->bios[sctx->curr]->next_free = -1;
			sctx->bios[sctx->curr]->page_count = 0;
			spin_unlock(&sctx->list_lock);
		} else {
			spin_unlock(&sctx->list_lock);
			wait_event(sctx->list_wait, sctx->first_free != -1);
		}
	}
	sbio = sctx->bios[sctx->curr];
	if (sbio->page_count == 0) {
		struct bio *bio;

		sbio->physical = spage->physical;
		sbio->logical = spage->logical;
		sbio->dev = spage->dev;
		bio = sbio->bio;
		if (!bio) {
			bio = btrfs_io_bio_alloc(sctx->pages_per_rd_bio);
			sbio->bio = bio;
		}

		bio->bi_private = sbio;
		bio->bi_end_io = scrub_bio_end_io;
		bio_set_dev(bio, sbio->dev->bdev);
		bio->bi_iter.bi_sector = sbio->physical >> 9;
		bio->bi_opf = REQ_OP_READ;
		sbio->status = 0;
	} else if (sbio->physical + sbio->page_count * PAGE_SIZE !=
		   spage->physical ||
		   sbio->logical + sbio->page_count * PAGE_SIZE !=
		   spage->logical ||
		   sbio->dev != spage->dev) {
		scrub_submit(sctx);
		goto again;
	}

	sbio->pagev[sbio->page_count] = spage;
	ret = bio_add_page(sbio->bio, spage->page, PAGE_SIZE, 0);
	if (ret != PAGE_SIZE) {
		if (sbio->page_count < 1) {
			bio_put(sbio->bio);
			sbio->bio = NULL;
			return -EIO;
		}
		scrub_submit(sctx);
		goto again;
	}

	scrub_block_get(sblock); /* one for the page added to the bio */
	atomic_inc(&sblock->outstanding_pages);
	sbio->page_count++;
	if (sbio->page_count == sctx->pages_per_rd_bio)
		scrub_submit(sctx);

	return 0;
}