static int scrub_pages(struct scrub_ctx *sctx, u64 logical, u64 len,
		       u64 physical, struct btrfs_device *dev, u64 flags,
		       u64 gen, int mirror_num, u8 *csum, int force,
		       u64 physical_for_dev_replace)
{
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
		scrub_page_get(spage);
		sblock->pagev[index] = spage;
		spage->sblock = sblock;
		spage->dev = dev;
		spage->flags = flags;
		spage->generation = gen;
		spage->logical = logical;
		spage->physical = physical;
		spage->physical_for_dev_replace = physical_for_dev_replace;
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
		physical_for_dev_replace += l;
	}

	WARN_ON(sblock->page_count == 0);
	if (test_bit(BTRFS_DEV_STATE_MISSING, &dev->dev_state)) {
		/*
		 * This case should only be hit for RAID 5/6 device replace. See
		 * the comment in scrub_missing_raid56_pages() for details.
		 */
		scrub_missing_raid56_pages(sblock);
	} else {
		for (index = 0; index < sblock->page_count; index++) {
			struct scrub_page *spage = sblock->pagev[index];
			int ret;

			ret = scrub_add_page_to_rd_bio(sctx, spage);
			if (ret) {
				scrub_block_put(sblock);
				return ret;
			}
		}

		if (force)
			scrub_submit(sctx);
	}

	/* last one frees, either here or in bio completion for last page */
	scrub_block_put(sblock);
	return 0;
}