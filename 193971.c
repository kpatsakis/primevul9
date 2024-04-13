bool bio_attempt_front_merge(struct request_queue *q, struct request *req,
			     struct bio *bio)
{
	const int ff = bio->bi_opf & REQ_FAILFAST_MASK;

	if (!ll_front_merge_fn(q, req, bio))
		return false;

	trace_block_bio_frontmerge(q, req, bio);

	if ((req->cmd_flags & REQ_FAILFAST_MASK) != ff)
		blk_rq_set_mixed_merge(req);

	bio->bi_next = req->bio;
	req->bio = bio;

	req->__sector = bio->bi_iter.bi_sector;
	req->__data_len += bio->bi_iter.bi_size;

	blk_account_io_start(req, false);
	return true;
}