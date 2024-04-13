static int scrub_setup_recheck_block(struct scrub_block *original_sblock,
				     struct scrub_block *sblocks_for_recheck)
{
	struct scrub_ctx *sctx = original_sblock->sctx;
	struct btrfs_fs_info *fs_info = sctx->fs_info;
	u64 length = original_sblock->page_count * PAGE_SIZE;
	u64 logical = original_sblock->pagev[0]->logical;
	u64 generation = original_sblock->pagev[0]->generation;
	u64 flags = original_sblock->pagev[0]->flags;
	u64 have_csum = original_sblock->pagev[0]->have_csum;
	struct scrub_recover *recover;
	struct btrfs_bio *bbio;
	u64 sublen;
	u64 mapped_length;
	u64 stripe_offset;
	int stripe_index;
	int page_index = 0;
	int mirror_index;
	int nmirrors;
	int ret;

	/*
	 * note: the two members refs and outstanding_pages
	 * are not used (and not set) in the blocks that are used for
	 * the recheck procedure
	 */

	while (length > 0) {
		sublen = min_t(u64, length, PAGE_SIZE);
		mapped_length = sublen;
		bbio = NULL;

		/*
		 * with a length of PAGE_SIZE, each returned stripe
		 * represents one mirror
		 */
		btrfs_bio_counter_inc_blocked(fs_info);
		ret = btrfs_map_sblock(fs_info, BTRFS_MAP_GET_READ_MIRRORS,
				logical, &mapped_length, &bbio);
		if (ret || !bbio || mapped_length < sublen) {
			btrfs_put_bbio(bbio);
			btrfs_bio_counter_dec(fs_info);
			return -EIO;
		}

		recover = kzalloc(sizeof(struct scrub_recover), GFP_NOFS);
		if (!recover) {
			btrfs_put_bbio(bbio);
			btrfs_bio_counter_dec(fs_info);
			return -ENOMEM;
		}

		refcount_set(&recover->refs, 1);
		recover->bbio = bbio;
		recover->map_length = mapped_length;

		BUG_ON(page_index >= SCRUB_MAX_PAGES_PER_BLOCK);

		nmirrors = min(scrub_nr_raid_mirrors(bbio), BTRFS_MAX_MIRRORS);

		for (mirror_index = 0; mirror_index < nmirrors;
		     mirror_index++) {
			struct scrub_block *sblock;
			struct scrub_page *page;

			sblock = sblocks_for_recheck + mirror_index;
			sblock->sctx = sctx;

			page = kzalloc(sizeof(*page), GFP_NOFS);
			if (!page) {
leave_nomem:
				spin_lock(&sctx->stat_lock);
				sctx->stat.malloc_errors++;
				spin_unlock(&sctx->stat_lock);
				scrub_put_recover(fs_info, recover);
				return -ENOMEM;
			}
			scrub_page_get(page);
			sblock->pagev[page_index] = page;
			page->sblock = sblock;
			page->flags = flags;
			page->generation = generation;
			page->logical = logical;
			page->have_csum = have_csum;
			if (have_csum)
				memcpy(page->csum,
				       original_sblock->pagev[0]->csum,
				       sctx->csum_size);

			scrub_stripe_index_and_offset(logical,
						      bbio->map_type,
						      bbio->raid_map,
						      mapped_length,
						      bbio->num_stripes -
						      bbio->num_tgtdevs,
						      mirror_index,
						      &stripe_index,
						      &stripe_offset);
			page->physical = bbio->stripes[stripe_index].physical +
					 stripe_offset;
			page->dev = bbio->stripes[stripe_index].dev;

			BUG_ON(page_index >= original_sblock->page_count);
			page->physical_for_dev_replace =
				original_sblock->pagev[page_index]->
				physical_for_dev_replace;
			/* for missing devices, dev->bdev is NULL */
			page->mirror_num = mirror_index + 1;
			sblock->page_count++;
			page->page = alloc_page(GFP_NOFS);
			if (!page->page)
				goto leave_nomem;

			scrub_get_recover(recover);
			page->recover = recover;
		}
		scrub_put_recover(fs_info, recover);
		length -= sublen;
		logical += sublen;
		page_index++;
	}

	return 0;
}