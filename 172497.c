static void scrub_recheck_block_on_raid56(struct btrfs_fs_info *fs_info,
					  struct scrub_block *sblock)
{
	struct scrub_page *first_page = sblock->pagev[0];
	struct bio *bio;
	int page_num;

	/* All pages in sblock belong to the same stripe on the same device. */
	ASSERT(first_page->dev);
	if (!first_page->dev->bdev)
		goto out;

	bio = btrfs_io_bio_alloc(BIO_MAX_PAGES);
	bio_set_dev(bio, first_page->dev->bdev);

	for (page_num = 0; page_num < sblock->page_count; page_num++) {
		struct scrub_page *page = sblock->pagev[page_num];

		WARN_ON(!page->page);
		bio_add_page(bio, page->page, PAGE_SIZE, 0);
	}

	if (scrub_submit_raid56_bio_wait(fs_info, bio, first_page)) {
		bio_put(bio);
		goto out;
	}

	bio_put(bio);

	scrub_recheck_block_checksum(sblock);

	return;
out:
	for (page_num = 0; page_num < sblock->page_count; page_num++)
		sblock->pagev[page_num]->io_error = 1;

	sblock->no_io_error_seen = 0;
}