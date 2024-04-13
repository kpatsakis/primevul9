bool bio_attempt_back_merge(struct request_queue *q, struct request *req,
			    struct bio *bio)
{
	const int ff = bio->bi_opf & REQ_FAILFAST_MASK;

	if (!ll_back_merge_fn(q, req, bio))
		return false;

	trace_block_bio_backmerge(q, req, bio);

	if ((req->cmd_flags & REQ_FAILFAST_MASK) != ff)
		blk_rq_set_mixed_merge(req);

	req->biotail->bi_next = bio;
	req->biotail = bio;
	req->__data_len += bio->bi_iter.bi_size;

	blk_account_io_start(req, false);
	return true;
}