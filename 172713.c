static void scrub_recheck_block(struct btrfs_fs_info *fs_info,
				struct scrub_block *sblock,
				int retry_failed_mirror)
{
	int page_num;

	sblock->no_io_error_seen = 1;

	/* short cut for raid56 */
	if (!retry_failed_mirror && scrub_is_page_on_raid56(sblock->pagev[0]))
		return scrub_recheck_block_on_raid56(fs_info, sblock);

	for (page_num = 0; page_num < sblock->page_count; page_num++) {
		struct bio *bio;
		struct scrub_page *page = sblock->pagev[page_num];

		if (page->dev->bdev == NULL) {
			page->io_error = 1;
			sblock->no_io_error_seen = 0;
			continue;
		}

		WARN_ON(!page->page);
		bio = btrfs_io_bio_alloc(1);
		bio_set_dev(bio, page->dev->bdev);

		bio_add_page(bio, page->page, PAGE_SIZE, 0);
		bio->bi_iter.bi_sector = page->physical >> 9;
		bio->bi_opf = REQ_OP_READ;

		if (btrfsic_submit_bio_wait(bio)) {
			page->io_error = 1;
			sblock->no_io_error_seen = 0;
		}

		bio_put(bio);
	}

	if (sblock->no_io_error_seen)
		scrub_recheck_block_checksum(sblock);
}