blk_qc_t direct_make_request(struct bio *bio)
{
	struct request_queue *q = bio->bi_disk->queue;
	bool nowait = bio->bi_opf & REQ_NOWAIT;
	blk_qc_t ret;

	if (!generic_make_request_checks(bio))
		return BLK_QC_T_NONE;

	if (unlikely(blk_queue_enter(q, nowait ? BLK_MQ_REQ_NOWAIT : 0))) {
		if (nowait && !blk_queue_dying(q))
			bio->bi_status = BLK_STS_AGAIN;
		else
			bio->bi_status = BLK_STS_IOERR;
		bio_endio(bio);
		return BLK_QC_T_NONE;
	}

	ret = q->make_request_fn(q, bio);
	blk_queue_exit(q);
	return ret;
}