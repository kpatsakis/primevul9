int f2fs_merge_page_bio(struct f2fs_io_info *fio)
{
	struct bio *bio = *fio->bio;
	struct page *page = fio->encrypted_page ?
			fio->encrypted_page : fio->page;

	if (!f2fs_is_valid_blkaddr(fio->sbi, fio->new_blkaddr,
			__is_meta_io(fio) ? META_GENERIC : DATA_GENERIC))
		return -EFSCORRUPTED;

	trace_f2fs_submit_page_bio(page, fio);
	f2fs_trace_ios(fio, 0);

	if (bio && (*fio->last_block + 1 != fio->new_blkaddr ||
			!__same_bdev(fio->sbi, fio->new_blkaddr, bio))) {
		__submit_bio(fio->sbi, bio, fio->type);
		bio = NULL;
	}
alloc_new:
	if (!bio) {
		bio = __bio_alloc(fio->sbi, fio->new_blkaddr, fio->io_wbc,
				BIO_MAX_PAGES, false, fio->type, fio->temp);
		bio_set_op_attrs(bio, fio->op, fio->op_flags);
	}

	if (bio_add_page(bio, page, PAGE_SIZE, 0) < PAGE_SIZE) {
		__submit_bio(fio->sbi, bio, fio->type);
		bio = NULL;
		goto alloc_new;
	}

	if (fio->io_wbc)
		wbc_account_io(fio->io_wbc, page, PAGE_SIZE);

	inc_page_count(fio->sbi, WB_DATA_TYPE(page));

	*fio->last_block = fio->new_blkaddr;
	*fio->bio = bio;

	return 0;
}