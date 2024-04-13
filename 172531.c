static void btrfs_end_bio(struct bio *bio)
{
	struct btrfs_bio *bbio = bio->bi_private;
	int is_orig_bio = 0;

	if (bio->bi_status) {
		atomic_inc(&bbio->error);
		if (bio->bi_status == BLK_STS_IOERR ||
		    bio->bi_status == BLK_STS_TARGET) {
			unsigned int stripe_index =
				btrfs_io_bio(bio)->stripe_index;
			struct btrfs_device *dev;

			BUG_ON(stripe_index >= bbio->num_stripes);
			dev = bbio->stripes[stripe_index].dev;
			if (dev->bdev) {
				if (bio_op(bio) == REQ_OP_WRITE)
					btrfs_dev_stat_inc_and_print(dev,
						BTRFS_DEV_STAT_WRITE_ERRS);
				else
					btrfs_dev_stat_inc_and_print(dev,
						BTRFS_DEV_STAT_READ_ERRS);
				if (bio->bi_opf & REQ_PREFLUSH)
					btrfs_dev_stat_inc_and_print(dev,
						BTRFS_DEV_STAT_FLUSH_ERRS);
			}
		}
	}

	if (bio == bbio->orig_bio)
		is_orig_bio = 1;

	btrfs_bio_counter_dec(bbio->fs_info);

	if (atomic_dec_and_test(&bbio->stripes_pending)) {
		if (!is_orig_bio) {
			bio_put(bio);
			bio = bbio->orig_bio;
		}

		btrfs_io_bio(bio)->mirror_num = bbio->mirror_num;
		/* only send an error to the higher layers if it is
		 * beyond the tolerance of the btrfs bio
		 */
		if (atomic_read(&bbio->error) > bbio->max_errors) {
			bio->bi_status = BLK_STS_IOERR;
		} else {
			/*
			 * this bio is actually up to date, we didn't
			 * go over the max number of errors
			 */
			bio->bi_status = BLK_STS_OK;
		}

		btrfs_end_bbio(bbio, bio);
	} else if (!is_orig_bio) {
		bio_put(bio);
	}
}