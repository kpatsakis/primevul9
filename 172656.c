static void scrub_wr_bio_end_io(struct bio *bio)
{
	struct scrub_bio *sbio = bio->bi_private;
	struct btrfs_fs_info *fs_info = sbio->dev->fs_info;

	sbio->status = bio->bi_status;
	sbio->bio = bio;

	btrfs_init_work(&sbio->work, btrfs_scrubwrc_helper,
			 scrub_wr_bio_end_io_worker, NULL, NULL);
	btrfs_queue_work(fs_info->scrub_wr_completion_workers, &sbio->work);
}