static void req_bio_endio(struct request *rq, struct bio *bio,
			  unsigned int nbytes, blk_status_t error)
{
	if (error)
		bio->bi_status = error;

	if (unlikely(rq->rq_flags & RQF_QUIET))
		bio_set_flag(bio, BIO_QUIET);

	bio_advance(bio, nbytes);

	/* don't actually finish bio if it's part of flush sequence */
	if (bio->bi_iter.bi_size == 0 && !(rq->rq_flags & RQF_FLUSH_SEQ))
		bio_endio(bio);
}