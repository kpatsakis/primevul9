static int scrub_pages_for_parity(struct scrub_parity *sparity,
				  u64 logical, u64 len,
				  u64 physical, struct btrfs_device *dev,
				  u64 flags, u64 gen, int mirror_num, u8 *csum)
{
	struct scrub_ctx *sctx = sparity->sctx;
	struct scrub_block *sblock;
	int index;

	sblock = kzalloc(sizeof(*sblock), GFP_KERNEL);
	if (!sblock) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.malloc_errors++;
		spin_unlock(&sctx->stat_lock);
		return -ENOMEM;
	}

	/* one ref inside this function, plus one for each page added to
	 * a bio later on */
	refcount_set(&sblock->refs, 1);
	sblock->sctx = sctx;
	sblock->no_io_error_seen = 1;
	sblock->sparity = sparity;
	scrub_parity_get(sparity);

	for (index = 0; len > 0; index++) {
		struct scrub_page *spage;
		u64 l = min_t(u64, len, PAGE_SIZE);

		spage = kzalloc(sizeof(*spage), GFP_KERNEL);
		if (!spage) {
leave_nomem:
			spin_lock(&sctx->stat_lock);
			sctx->stat.malloc_errors++;
			spin_unlock(&sctx->stat_lock);
			scrub_block_put(sblock);
			return -ENOMEM;
		}
		BUG_ON(index >= SCRUB_MAX_PAGES_PER_BLOCK);
		/* For scrub block */
		scrub_page_get(spage);
		sblock->pagev[index] = spage;
		/* For scrub parity */
		scrub_page_get(spage);
		list_add_tail(&spage->list, &sparity->spages);
		spage->sblock = sblock;
		spage->dev = dev;
		spage->flags = flags;
		spage->generation = gen;
		spage->logical = logical;
		spage->physical = physical;
		spage->mirror_num = mirror_num;
		if (csum) {
			spage->have_csum = 1;
			memcpy(spage->csum, csum, sctx->csum_size);
		} else {
			spage->have_csum = 0;
		}
		sblock->page_count++;
		spage->page = alloc_page(GFP_KERNEL);
		if (!spage->page)
			goto leave_nomem;
		len -= l;
		logical += l;
		physical += l;
	}

	WARN_ON(sblock->page_count == 0);
	for (index = 0; index < sblock->page_count; index++) {
		struct scrub_page *spage = sblock->pagev[index];
		int ret;

		ret = scrub_add_page_to_rd_bio(sctx, spage);
		if (ret) {
			scrub_block_put(sblock);
			return ret;
		}
	}

	/* last one frees, either here or in bio completion for last page */
	scrub_block_put(sblock);
	return 0;
}