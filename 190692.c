void f2fs_submit_page_write(struct f2fs_io_info *fio)
{
	struct f2fs_sb_info *sbi = fio->sbi;
	enum page_type btype = PAGE_TYPE_OF_BIO(fio->type);
	struct f2fs_bio_info *io = sbi->write_io[btype] + fio->temp;
	struct page *bio_page;

	f2fs_bug_on(sbi, is_read_io(fio->op));

	down_write(&io->io_rwsem);
next:
	if (fio->in_list) {
		spin_lock(&io->io_lock);
		if (list_empty(&io->io_list)) {
			spin_unlock(&io->io_lock);
			goto out;
		}
		fio = list_first_entry(&io->io_list,
						struct f2fs_io_info, list);
		list_del(&fio->list);
		spin_unlock(&io->io_lock);
	}

	verify_fio_blkaddr(fio);

	bio_page = fio->encrypted_page ? fio->encrypted_page : fio->page;

	/* set submitted = true as a return value */
	fio->submitted = true;

	inc_page_count(sbi, WB_DATA_TYPE(bio_page));

	if (io->bio && (io->last_block_in_bio != fio->new_blkaddr - 1 ||
	    (io->fio.op != fio->op || io->fio.op_flags != fio->op_flags) ||
			!__same_bdev(sbi, fio->new_blkaddr, io->bio)))
		__submit_merged_bio(io);
alloc_new:
	if (io->bio == NULL) {
		if ((fio->type == DATA || fio->type == NODE) &&
				fio->new_blkaddr & F2FS_IO_SIZE_MASK(sbi)) {
			dec_page_count(sbi, WB_DATA_TYPE(bio_page));
			fio->retry = true;
			goto skip;
		}
		io->bio = __bio_alloc(sbi, fio->new_blkaddr, fio->io_wbc,
						BIO_MAX_PAGES, false,
						fio->type, fio->temp);
		io->fio = *fio;
	}

	if (bio_add_page(io->bio, bio_page, PAGE_SIZE, 0) < PAGE_SIZE) {
		__submit_merged_bio(io);
		goto alloc_new;
	}

	if (fio->io_wbc)
		wbc_account_io(fio->io_wbc, bio_page, PAGE_SIZE);

	io->last_block_in_bio = fio->new_blkaddr;
	f2fs_trace_ios(fio, 0);

	trace_f2fs_submit_page_write(fio->page, fio);
skip:
	if (fio->in_list)
		goto next;
out:
	if (is_sbi_flag_set(sbi, SBI_IS_SHUTDOWN) ||
				f2fs_is_checkpoint_ready(sbi))
		__submit_merged_bio(io);
	up_write(&io->io_rwsem);
}