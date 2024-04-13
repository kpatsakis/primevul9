blk_status_t btrfs_map_bio(struct btrfs_fs_info *fs_info, struct bio *bio,
			   int mirror_num, int async_submit)
{
	struct btrfs_device *dev;
	struct bio *first_bio = bio;
	u64 logical = (u64)bio->bi_iter.bi_sector << 9;
	u64 length = 0;
	u64 map_length;
	int ret;
	int dev_nr;
	int total_devs;
	struct btrfs_bio *bbio = NULL;

	length = bio->bi_iter.bi_size;
	map_length = length;

	btrfs_bio_counter_inc_blocked(fs_info);
	ret = __btrfs_map_block(fs_info, btrfs_op(bio), logical,
				&map_length, &bbio, mirror_num, 1);
	if (ret) {
		btrfs_bio_counter_dec(fs_info);
		return errno_to_blk_status(ret);
	}

	total_devs = bbio->num_stripes;
	bbio->orig_bio = first_bio;
	bbio->private = first_bio->bi_private;
	bbio->end_io = first_bio->bi_end_io;
	bbio->fs_info = fs_info;
	atomic_set(&bbio->stripes_pending, bbio->num_stripes);

	if ((bbio->map_type & BTRFS_BLOCK_GROUP_RAID56_MASK) &&
	    ((bio_op(bio) == REQ_OP_WRITE) || (mirror_num > 1))) {
		/* In this case, map_length has been set to the length of
		   a single stripe; not the whole write */
		if (bio_op(bio) == REQ_OP_WRITE) {
			ret = raid56_parity_write(fs_info, bio, bbio,
						  map_length);
		} else {
			ret = raid56_parity_recover(fs_info, bio, bbio,
						    map_length, mirror_num, 1);
		}

		btrfs_bio_counter_dec(fs_info);
		return errno_to_blk_status(ret);
	}

	if (map_length < length) {
		btrfs_crit(fs_info,
			   "mapping failed logical %llu bio len %llu len %llu",
			   logical, length, map_length);
		BUG();
	}

	for (dev_nr = 0; dev_nr < total_devs; dev_nr++) {
		dev = bbio->stripes[dev_nr].dev;
		if (!dev || !dev->bdev || test_bit(BTRFS_DEV_STATE_MISSING,
						   &dev->dev_state) ||
		    (bio_op(first_bio) == REQ_OP_WRITE &&
		    !test_bit(BTRFS_DEV_STATE_WRITEABLE, &dev->dev_state))) {
			bbio_error(bbio, first_bio, logical);
			continue;
		}

		if (dev_nr < total_devs - 1)
			bio = btrfs_bio_clone(first_bio);
		else
			bio = first_bio;

		submit_stripe_bio(bbio, bio, bbio->stripes[dev_nr].physical,
				  dev_nr, async_submit);
	}
	btrfs_bio_counter_dec(fs_info);
	return BLK_STS_OK;
}