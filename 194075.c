generic_make_request_checks(struct bio *bio)
{
	struct request_queue *q;
	int nr_sectors = bio_sectors(bio);
	blk_status_t status = BLK_STS_IOERR;
	char b[BDEVNAME_SIZE];

	might_sleep();

	q = bio->bi_disk->queue;
	if (unlikely(!q)) {
		printk(KERN_ERR
		       "generic_make_request: Trying to access "
			"nonexistent block-device %s (%Lu)\n",
			bio_devname(bio, b), (long long)bio->bi_iter.bi_sector);
		goto end_io;
	}

	/*
	 * For a REQ_NOWAIT based request, return -EOPNOTSUPP
	 * if queue is not a request based queue.
	 */
	if ((bio->bi_opf & REQ_NOWAIT) && !queue_is_mq(q))
		goto not_supported;

	if (should_fail_bio(bio))
		goto end_io;

	if (bio->bi_partno) {
		if (unlikely(blk_partition_remap(bio)))
			goto end_io;
	} else {
		if (unlikely(bio_check_ro(bio, &bio->bi_disk->part0)))
			goto end_io;
		if (unlikely(bio_check_eod(bio, get_capacity(bio->bi_disk))))
			goto end_io;
	}

	/*
	 * Filter flush bio's early so that make_request based
	 * drivers without flush support don't have to worry
	 * about them.
	 */
	if (op_is_flush(bio->bi_opf) &&
	    !test_bit(QUEUE_FLAG_WC, &q->queue_flags)) {
		bio->bi_opf &= ~(REQ_PREFLUSH | REQ_FUA);
		if (!nr_sectors) {
			status = BLK_STS_OK;
			goto end_io;
		}
	}

	if (!test_bit(QUEUE_FLAG_POLL, &q->queue_flags))
		bio->bi_opf &= ~REQ_HIPRI;

	switch (bio_op(bio)) {
	case REQ_OP_DISCARD:
		if (!blk_queue_discard(q))
			goto not_supported;
		break;
	case REQ_OP_SECURE_ERASE:
		if (!blk_queue_secure_erase(q))
			goto not_supported;
		break;
	case REQ_OP_WRITE_SAME:
		if (!q->limits.max_write_same_sectors)
			goto not_supported;
		break;
	case REQ_OP_ZONE_RESET:
		if (!blk_queue_is_zoned(q))
			goto not_supported;
		break;
	case REQ_OP_WRITE_ZEROES:
		if (!q->limits.max_write_zeroes_sectors)
			goto not_supported;
		break;
	default:
		break;
	}

	/*
	 * Various block parts want %current->io_context and lazy ioc
	 * allocation ends up trading a lot of pain for a small amount of
	 * memory.  Just allocate it upfront.  This may fail and block
	 * layer knows how to live with it.
	 */
	create_io_context(GFP_ATOMIC, q->node);

	if (!blkcg_bio_issue_check(q, bio))
		return false;

	if (!bio_flagged(bio, BIO_TRACE_COMPLETION)) {
		trace_block_bio_queue(q, bio);
		/* Now that enqueuing has been traced, we need to trace
		 * completion as well.
		 */
		bio_set_flag(bio, BIO_TRACE_COMPLETION);
	}
	return true;

not_supported:
	status = BLK_STS_NOTSUPP;
end_io:
	bio->bi_status = status;
	bio_endio(bio);
	return false;
}