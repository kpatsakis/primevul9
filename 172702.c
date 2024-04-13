static void scrub_missing_raid56_pages(struct scrub_block *sblock)
{
	struct scrub_ctx *sctx = sblock->sctx;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	u64 length = sblock->page_count * PAGE_SIZE;
	u64 logical = sblock->pagev[0]->logical;
	struct btrfs_bio *bbio = NULL;
	struct bio *bio;
	struct btrfs_raid_bio *rbio;
	int ret;
	int i;

	btrfs_bio_counter_inc_blocked(fs_info);
	ret = btrfs_map_sblock(fs_info, BTRFS_MAP_GET_READ_MIRRORS, logical,
			&length, &bbio);
	if (ret || !bbio || !bbio->raid_map)
		goto bbio_out;

	if (WARN_ON(!sctx->is_dev_replace ||
		    !(bbio->map_type & BTRFS_BLOCK_GROUP_RAID56_MASK))) {
		/*
		 * We shouldn't be scrubbing a missing device. Even for dev
		 * replace, we should only get here for RAID 5/6. We either
		 * managed to mount something with no mirrors remaining or
		 * there's a bug in scrub_remap_extent()/btrfs_map_block().
		 */
		goto bbio_out;
	}

	bio = btrfs_io_bio_alloc(0);
	bio->bi_iter.bi_sector = logical >> 9;
	bio->bi_private = sblock;
	bio->bi_end_io = scrub_missing_raid56_end_io;

	rbio = raid56_alloc_missing_rbio(fs_info, bio, bbio, length);
	if (!rbio)
		goto rbio_out;

	for (i = 0; i < sblock->page_count; i++) {
		struct scrub_page *spage = sblock->pagev[i];

		raid56_add_scrub_pages(rbio, spage->page, spage->logical);
	}

	btrfs_init_work(&sblock->work, btrfs_scrub_helper,
			scrub_missing_raid56_worker, NULL, NULL);
	scrub_block_get(sblock);
	scrub_pending_bio_inc(sctx);
	raid56_submit_missing_rbio(rbio);
	return;

rbio_out:
	bio_put(bio);
bbio_out:
	btrfs_bio_counter_dec(fs_info);
	btrfs_put_bbio(bbio);
	spin_lock(&sctx->stat_lock);
	sctx->stat.malloc_errors++;
	spin_unlock(&sctx->stat_lock);
}