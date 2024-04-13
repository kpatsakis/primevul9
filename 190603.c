static struct bio *__bio_alloc(struct f2fs_sb_info *sbi, block_t blk_addr,
				struct writeback_control *wbc,
				int npages, bool is_read,
				enum page_type type, enum temp_type temp)
{
	struct bio *bio;

	bio = f2fs_bio_alloc(sbi, npages, true);

	f2fs_target_device(sbi, blk_addr, bio);
	if (is_read) {
		bio->bi_end_io = f2fs_read_end_io;
		bio->bi_private = NULL;
	} else {
		bio->bi_end_io = f2fs_write_end_io;
		bio->bi_private = sbi;
		bio->bi_write_hint = f2fs_io_type_to_rw_hint(sbi, type, temp);
	}
	if (wbc)
		wbc_init_bio(wbc, bio);

	return bio;
}