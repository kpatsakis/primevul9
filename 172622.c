static int scrub_submit_raid56_bio_wait(struct btrfs_fs_info *fs_info,
					struct bio *bio,
					struct scrub_page *page)
{
	DECLARE_COMPLETION_ONSTACK(done);
	int ret;
	int mirror_num;

	bio->bi_iter.bi_sector = page->logical >> 9;
	bio->bi_private = &done;
	bio->bi_end_io = scrub_bio_wait_endio;

	mirror_num = page->sblock->pagev[0]->mirror_num;
	ret = raid56_parity_recover(fs_info, bio, page->recover->bbio,
				    page->recover->map_length,
				    mirror_num, 0);
	if (ret)
		return ret;

	wait_for_completion_io(&done);
	return blk_status_to_errno(bio->bi_status);
}