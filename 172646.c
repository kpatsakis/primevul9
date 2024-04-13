static int scrub_repair_page_from_good_copy(struct scrub_block *sblock_bad,
					    struct scrub_block *sblock_good,
					    int page_num, int force_write)
{
	struct scrub_page *page_bad = sblock_bad->pagev[page_num];
	struct scrub_page *page_good = sblock_good->pagev[page_num];
	struct btrfs_fs_info *fs_info = sblock_bad->sctx->fs_info;

	BUG_ON(page_bad->page == NULL);
	BUG_ON(page_good->page == NULL);
	if (force_write || sblock_bad->header_error ||
	    sblock_bad->checksum_error || page_bad->io_error) {
		struct bio *bio;
		int ret;

		if (!page_bad->dev->bdev) {
			btrfs_warn_rl(fs_info,
				"scrub_repair_page_from_good_copy(bdev == NULL) is unexpected");
			return -EIO;
		}

		bio = btrfs_io_bio_alloc(1);
		bio_set_dev(bio, page_bad->dev->bdev);
		bio->bi_iter.bi_sector = page_bad->physical >> 9;
		bio->bi_opf = REQ_OP_WRITE;

		ret = bio_add_page(bio, page_good->page, PAGE_SIZE, 0);
		if (PAGE_SIZE != ret) {
			bio_put(bio);
			return -EIO;
		}

		if (btrfsic_submit_bio_wait(bio)) {
			btrfs_dev_stat_inc_and_print(page_bad->dev,
				BTRFS_DEV_STAT_WRITE_ERRS);
			atomic64_inc(&fs_info->dev_replace.num_write_errors);
			bio_put(bio);
			return -EIO;
		}
		bio_put(bio);
	}

	return 0;
}