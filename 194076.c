blk_status_t blk_insert_cloned_request(struct request_queue *q, struct request *rq)
{
	if (blk_cloned_rq_check_limits(q, rq))
		return BLK_STS_IOERR;

	if (rq->rq_disk &&
	    should_fail_request(&rq->rq_disk->part0, blk_rq_bytes(rq)))
		return BLK_STS_IOERR;

	if (blk_queue_io_stat(q))
		blk_account_io_start(rq, true);

	/*
	 * Since we have a scheduler attached on the top device,
	 * bypass a potential scheduler on the bottom device for
	 * insert.
	 */
	return blk_mq_request_issue_directly(rq, true);
}