static void submit_stripe_bio(struct btrfs_bio *bbio, struct bio *bio,
			      u64 physical, int dev_nr, int async)
{
	struct btrfs_device *dev = bbio->stripes[dev_nr].dev;
	struct btrfs_fs_info *fs_info = bbio->fs_info;

	bio->bi_private = bbio;
	btrfs_io_bio(bio)->stripe_index = dev_nr;
	bio->bi_end_io = btrfs_end_bio;
	bio->bi_iter.bi_sector = physical >> 9;
	btrfs_debug_in_rcu(fs_info,
	"btrfs_map_bio: rw %d 0x%x, sector=%llu, dev=%lu (%s id %llu), size=%u",
		bio_op(bio), bio->bi_opf, (u64)bio->bi_iter.bi_sector,
		(u_long)dev->bdev->bd_dev, rcu_str_deref(dev->name), dev->devid,
		bio->bi_iter.bi_size);
	bio_set_dev(bio, dev->bdev);

	btrfs_bio_counter_inc_noblocked(fs_info);

	if (async)
		btrfs_schedule_bio(dev, bio);
	else
		btrfsic_submit_bio(bio);
}