void rq_flush_dcache_pages(struct request *rq)
{
	struct req_iterator iter;
	struct bio_vec bvec;

	rq_for_each_segment(bvec, rq, iter)
		flush_dcache_page(bvec.bv_page);
}