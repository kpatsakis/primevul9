static int scrub_handle_errored_block(struct scrub_block *sblock_to_check)
{
	struct scrub_ctx *sctx = sblock_to_check->sctx;
	struct btrfs_device *dev;
	struct btrfs_fs_info *fs_info;
	u64 logical;
	unsigned int failed_mirror_index;
	unsigned int is_metadata;
	unsigned int have_csum;
	struct scrub_block *sblocks_for_recheck; /* holds one for each mirror */
	struct scrub_block *sblock_bad;
	int ret;
	int mirror_index;
	int page_num;
	int success;
	bool full_stripe_locked;
	unsigned int nofs_flag;
	static DEFINE_RATELIMIT_STATE(_rs, DEFAULT_RATELIMIT_INTERVAL,
				      DEFAULT_RATELIMIT_BURST);

	BUG_ON(sblock_to_check->page_count < 1);
	fs_info = sctx->fs_info;
	if (sblock_to_check->pagev[0]->flags & BTRFS_EXTENT_FLAG_SUPER) {
		/*
		 * if we find an error in a super block, we just report it.
		 * They will get written with the next transaction commit
		 * anyway
		 */
		spin_lock(&sctx->stat_lock);
		++sctx->stat.super_errors;
		spin_unlock(&sctx->stat_lock);
		return 0;
	}
	logical = sblock_to_check->pagev[0]->logical;
	BUG_ON(sblock_to_check->pagev[0]->mirror_num < 1);
	failed_mirror_index = sblock_to_check->pagev[0]->mirror_num - 1;
	is_metadata = !(sblock_to_check->pagev[0]->flags &
			BTRFS_EXTENT_FLAG_DATA);
	have_csum = sblock_to_check->pagev[0]->have_csum;
	dev = sblock_to_check->pagev[0]->dev;

	/*
	 * We must use GFP_NOFS because the scrub task might be waiting for a
	 * worker task executing this function and in turn a transaction commit
	 * might be waiting the scrub task to pause (which needs to wait for all
	 * the worker tasks to complete before pausing).
	 * We do allocations in the workers through insert_full_stripe_lock()
	 * and scrub_add_page_to_wr_bio(), which happens down the call chain of
	 * this function.
	 */
	nofs_flag = memalloc_nofs_save();
	/*
	 * For RAID5/6, race can happen for a different device scrub thread.
	 * For data corruption, Parity and Data threads will both try
	 * to recovery the data.
	 * Race can lead to doubly added csum error, or even unrecoverable
	 * error.
	 */
	ret = lock_full_stripe(fs_info, logical, &full_stripe_locked);
	if (ret < 0) {
		memalloc_nofs_restore(nofs_flag);
		spin_lock(&sctx->stat_lock);
		if (ret == -ENOMEM)
			sctx->stat.malloc_errors++;
		sctx->stat.read_errors++;
		sctx->stat.uncorrectable_errors++;
		spin_unlock(&sctx->stat_lock);
		return ret;
	}

	/*
	 * read all mirrors one after the other. This includes to
	 * re-read the extent or metadata block that failed (that was
	 * the cause that this fixup code is called) another time,
	 * page by page this time in order to know which pages
	 * caused I/O errors and which ones are good (for all mirrors).
	 * It is the goal to handle the situation when more than one
	 * mirror contains I/O errors, but the errors do not
	 * overlap, i.e. the data can be repaired by selecting the
	 * pages from those mirrors without I/O error on the
	 * particular pages. One example (with blocks >= 2 * PAGE_SIZE)
	 * would be that mirror #1 has an I/O error on the first page,
	 * the second page is good, and mirror #2 has an I/O error on
	 * the second page, but the first page is good.
	 * Then the first page of the first mirror can be repaired by
	 * taking the first page of the second mirror, and the
	 * second page of the second mirror can be repaired by
	 * copying the contents of the 2nd page of the 1st mirror.
	 * One more note: if the pages of one mirror contain I/O
	 * errors, the checksum cannot be verified. In order to get
	 * the best data for repairing, the first attempt is to find
	 * a mirror without I/O errors and with a validated checksum.
	 * Only if this is not possible, the pages are picked from
	 * mirrors with I/O errors without considering the checksum.
	 * If the latter is the case, at the end, the checksum of the
	 * repaired area is verified in order to correctly maintain
	 * the statistics.
	 */

	sblocks_for_recheck = kcalloc(BTRFS_MAX_MIRRORS,
				      sizeof(*sblocks_for_recheck), GFP_KERNEL);
	if (!sblocks_for_recheck) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.malloc_errors++;
		sctx->stat.read_errors++;
		sctx->stat.uncorrectable_errors++;
		spin_unlock(&sctx->stat_lock);
		btrfs_dev_stat_inc_and_print(dev, BTRFS_DEV_STAT_READ_ERRS);
		goto out;
	}

	/* setup the context, map the logical blocks and alloc the pages */
	ret = scrub_setup_recheck_block(sblock_to_check, sblocks_for_recheck);
	if (ret) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.read_errors++;
		sctx->stat.uncorrectable_errors++;
		spin_unlock(&sctx->stat_lock);
		btrfs_dev_stat_inc_and_print(dev, BTRFS_DEV_STAT_READ_ERRS);
		goto out;
	}
	BUG_ON(failed_mirror_index >= BTRFS_MAX_MIRRORS);
	sblock_bad = sblocks_for_recheck + failed_mirror_index;

	/* build and submit the bios for the failed mirror, check checksums */
	scrub_recheck_block(fs_info, sblock_bad, 1);

	if (!sblock_bad->header_error && !sblock_bad->checksum_error &&
	    sblock_bad->no_io_error_seen) {
		/*
		 * the error disappeared after reading page by page, or
		 * the area was part of a huge bio and other parts of the
		 * bio caused I/O errors, or the block layer merged several
		 * read requests into one and the error is caused by a
		 * different bio (usually one of the two latter cases is
		 * the cause)
		 */
		spin_lock(&sctx->stat_lock);
		sctx->stat.unverified_errors++;
		sblock_to_check->data_corrected = 1;
		spin_unlock(&sctx->stat_lock);

		if (sctx->is_dev_replace)
			scrub_write_block_to_dev_replace(sblock_bad);
		goto out;
	}

	if (!sblock_bad->no_io_error_seen) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.read_errors++;
		spin_unlock(&sctx->stat_lock);
		if (__ratelimit(&_rs))
			scrub_print_warning("i/o error", sblock_to_check);
		btrfs_dev_stat_inc_and_print(dev, BTRFS_DEV_STAT_READ_ERRS);
	} else if (sblock_bad->checksum_error) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.csum_errors++;
		spin_unlock(&sctx->stat_lock);
		if (__ratelimit(&_rs))
			scrub_print_warning("checksum error", sblock_to_check);
		btrfs_dev_stat_inc_and_print(dev,
					     BTRFS_DEV_STAT_CORRUPTION_ERRS);
	} else if (sblock_bad->header_error) {
		spin_lock(&sctx->stat_lock);
		sctx->stat.verify_errors++;
		spin_unlock(&sctx->stat_lock);
		if (__ratelimit(&_rs))
			scrub_print_warning("checksum/header error",
					    sblock_to_check);
		if (sblock_bad->generation_error)
			btrfs_dev_stat_inc_and_print(dev,
				BTRFS_DEV_STAT_GENERATION_ERRS);
		else
			btrfs_dev_stat_inc_and_print(dev,
				BTRFS_DEV_STAT_CORRUPTION_ERRS);
	}

	if (sctx->readonly) {
		ASSERT(!sctx->is_dev_replace);
		goto out;
	}

	/*
	 * now build and submit the bios for the other mirrors, check
	 * checksums.
	 * First try to pick the mirror which is completely without I/O
	 * errors and also does not have a checksum error.
	 * If one is found, and if a checksum is present, the full block
	 * that is known to contain an error is rewritten. Afterwards
	 * the block is known to be corrected.
	 * If a mirror is found which is completely correct, and no
	 * checksum is present, only those pages are rewritten that had
	 * an I/O error in the block to be repaired, since it cannot be
	 * determined, which copy of the other pages is better (and it
	 * could happen otherwise that a correct page would be
	 * overwritten by a bad one).
	 */
	for (mirror_index = 0; ;mirror_index++) {
		struct scrub_block *sblock_other;

		if (mirror_index == failed_mirror_index)
			continue;

		/* raid56's mirror can be more than BTRFS_MAX_MIRRORS */
		if (!scrub_is_page_on_raid56(sblock_bad->pagev[0])) {
			if (mirror_index >= BTRFS_MAX_MIRRORS)
				break;
			if (!sblocks_for_recheck[mirror_index].page_count)
				break;

			sblock_other = sblocks_for_recheck + mirror_index;
		} else {
			struct scrub_recover *r = sblock_bad->pagev[0]->recover;
			int max_allowed = r->bbio->num_stripes -
						r->bbio->num_tgtdevs;

			if (mirror_index >= max_allowed)
				break;
			if (!sblocks_for_recheck[1].page_count)
				break;

			ASSERT(failed_mirror_index == 0);
			sblock_other = sblocks_for_recheck + 1;
			sblock_other->pagev[0]->mirror_num = 1 + mirror_index;
		}

		/* build and submit the bios, check checksums */
		scrub_recheck_block(fs_info, sblock_other, 0);

		if (!sblock_other->header_error &&
		    !sblock_other->checksum_error &&
		    sblock_other->no_io_error_seen) {
			if (sctx->is_dev_replace) {
				scrub_write_block_to_dev_replace(sblock_other);
				goto corrected_error;
			} else {
				ret = scrub_repair_block_from_good_copy(
						sblock_bad, sblock_other);
				if (!ret)
					goto corrected_error;
			}
		}
	}

	if (sblock_bad->no_io_error_seen && !sctx->is_dev_replace)
		goto did_not_correct_error;

	/*
	 * In case of I/O errors in the area that is supposed to be
	 * repaired, continue by picking good copies of those pages.
	 * Select the good pages from mirrors to rewrite bad pages from
	 * the area to fix. Afterwards verify the checksum of the block
	 * that is supposed to be repaired. This verification step is
	 * only done for the purpose of statistic counting and for the
	 * final scrub report, whether errors remain.
	 * A perfect algorithm could make use of the checksum and try
	 * all possible combinations of pages from the different mirrors
	 * until the checksum verification succeeds. For example, when
	 * the 2nd page of mirror #1 faces I/O errors, and the 2nd page
	 * of mirror #2 is readable but the final checksum test fails,
	 * then the 2nd page of mirror #3 could be tried, whether now
	 * the final checksum succeeds. But this would be a rare
	 * exception and is therefore not implemented. At least it is
	 * avoided that the good copy is overwritten.
	 * A more useful improvement would be to pick the sectors
	 * without I/O error based on sector sizes (512 bytes on legacy
	 * disks) instead of on PAGE_SIZE. Then maybe 512 byte of one
	 * mirror could be repaired by taking 512 byte of a different
	 * mirror, even if other 512 byte sectors in the same PAGE_SIZE
	 * area are unreadable.
	 */
	success = 1;
	for (page_num = 0; page_num < sblock_bad->page_count;
	     page_num++) {
		struct scrub_page *page_bad = sblock_bad->pagev[page_num];
		struct scrub_block *sblock_other = NULL;

		/* skip no-io-error page in scrub */
		if (!page_bad->io_error && !sctx->is_dev_replace)
			continue;

		if (scrub_is_page_on_raid56(sblock_bad->pagev[0])) {
			/*
			 * In case of dev replace, if raid56 rebuild process
			 * didn't work out correct data, then copy the content
			 * in sblock_bad to make sure target device is identical
			 * to source device, instead of writing garbage data in
			 * sblock_for_recheck array to target device.
			 */
			sblock_other = NULL;
		} else if (page_bad->io_error) {
			/* try to find no-io-error page in mirrors */
			for (mirror_index = 0;
			     mirror_index < BTRFS_MAX_MIRRORS &&
			     sblocks_for_recheck[mirror_index].page_count > 0;
			     mirror_index++) {
				if (!sblocks_for_recheck[mirror_index].
				    pagev[page_num]->io_error) {
					sblock_other = sblocks_for_recheck +
						       mirror_index;
					break;
				}
			}
			if (!sblock_other)
				success = 0;
		}

		if (sctx->is_dev_replace) {
			/*
			 * did not find a mirror to fetch the page
			 * from. scrub_write_page_to_dev_replace()
			 * handles this case (page->io_error), by
			 * filling the block with zeros before
			 * submitting the write request
			 */
			if (!sblock_other)
				sblock_other = sblock_bad;

			if (scrub_write_page_to_dev_replace(sblock_other,
							    page_num) != 0) {
				atomic64_inc(
					&fs_info->dev_replace.num_write_errors);
				success = 0;
			}
		} else if (sblock_other) {
			ret = scrub_repair_page_from_good_copy(sblock_bad,
							       sblock_other,
							       page_num, 0);
			if (0 == ret)
				page_bad->io_error = 0;
			else
				success = 0;
		}
	}

	if (success && !sctx->is_dev_replace) {
		if (is_metadata || have_csum) {
			/*
			 * need to verify the checksum now that all
			 * sectors on disk are repaired (the write
			 * request for data to be repaired is on its way).
			 * Just be lazy and use scrub_recheck_block()
			 * which re-reads the data before the checksum
			 * is verified, but most likely the data comes out
			 * of the page cache.
			 */
			scrub_recheck_block(fs_info, sblock_bad, 1);
			if (!sblock_bad->header_error &&
			    !sblock_bad->checksum_error &&
			    sblock_bad->no_io_error_seen)
				goto corrected_error;
			else
				goto did_not_correct_error;
		} else {
corrected_error:
			spin_lock(&sctx->stat_lock);
			sctx->stat.corrected_errors++;
			sblock_to_check->data_corrected = 1;
			spin_unlock(&sctx->stat_lock);
			btrfs_err_rl_in_rcu(fs_info,
				"fixed up error at logical %llu on dev %s",
				logical, rcu_str_deref(dev->name));
		}
	} else {
did_not_correct_error:
		spin_lock(&sctx->stat_lock);
		sctx->stat.uncorrectable_errors++;
		spin_unlock(&sctx->stat_lock);
		btrfs_err_rl_in_rcu(fs_info,
			"unable to fixup (regular) error at logical %llu on dev %s",
			logical, rcu_str_deref(dev->name));
	}

out:
	if (sblocks_for_recheck) {
		for (mirror_index = 0; mirror_index < BTRFS_MAX_MIRRORS;
		     mirror_index++) {
			struct scrub_block *sblock = sblocks_for_recheck +
						     mirror_index;
			struct scrub_recover *recover;
			int page_index;

			for (page_index = 0; page_index < sblock->page_count;
			     page_index++) {
				sblock->pagev[page_index]->sblock = NULL;
				recover = sblock->pagev[page_index]->recover;
				if (recover) {
					scrub_put_recover(fs_info, recover);
					sblock->pagev[page_index]->recover =
									NULL;
				}
				scrub_page_put(sblock->pagev[page_index]);
			}
		}
		kfree(sblocks_for_recheck);
	}

	ret = unlock_full_stripe(fs_info, logical, full_stripe_locked);
	memalloc_nofs_restore(nofs_flag);
	if (ret < 0)
		return ret;
	return 0;
}