void blk_init_request_from_bio(struct request *req, struct bio *bio)
{
	if (bio->bi_opf & REQ_RAHEAD)
		req->cmd_flags |= REQ_FAILFAST_MASK;

	req->__sector = bio->bi_iter.bi_sector;
	req->ioprio = bio_prio(bio);
	req->write_hint = bio->bi_write_hint;
	blk_rq_bio_prep(req->q, req, bio);
}