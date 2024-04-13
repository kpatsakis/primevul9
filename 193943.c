static inline int blk_partition_remap(struct bio *bio)
{
	struct hd_struct *p;
	int ret = -EIO;

	rcu_read_lock();
	p = __disk_get_part(bio->bi_disk, bio->bi_partno);
	if (unlikely(!p))
		goto out;
	if (unlikely(should_fail_request(p, bio->bi_iter.bi_size)))
		goto out;
	if (unlikely(bio_check_ro(bio, p)))
		goto out;

	/*
	 * Zone reset does not include bi_size so bio_sectors() is always 0.
	 * Include a test for the reset op code and perform the remap if needed.
	 */
	if (bio_sectors(bio) || bio_op(bio) == REQ_OP_ZONE_RESET) {
		if (bio_check_eod(bio, part_nr_sects_read(p)))
			goto out;
		bio->bi_iter.bi_sector += p->start_sect;
		trace_block_bio_remap(bio->bi_disk->queue, bio, part_devt(p),
				      bio->bi_iter.bi_sector - p->start_sect);
	}
	bio->bi_partno = 0;
	ret = 0;
out:
	rcu_read_unlock();
	return ret;
}