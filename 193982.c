void blk_rq_bio_prep(struct request_queue *q, struct request *rq,
		     struct bio *bio)
{
	if (bio_has_data(bio))
		rq->nr_phys_segments = bio_phys_segments(q, bio);
	else if (bio_op(bio) == REQ_OP_DISCARD)
		rq->nr_phys_segments = 1;

	rq->__data_len = bio->bi_iter.bi_size;
	rq->bio = rq->biotail = bio;

	if (bio->bi_disk)
		rq->rq_disk = bio->bi_disk;
}