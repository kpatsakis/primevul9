static inline bool blk_do_io_stat(struct request *rq)
{
	return rq->rq_disk &&
	       (rq->rq_flags & RQF_IO_STAT) &&
		!blk_rq_is_passthrough(rq);
}