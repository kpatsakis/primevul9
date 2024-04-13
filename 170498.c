static void skcipher_free_async_sgls(struct skcipher_async_req *sreq)
{
	struct skcipher_async_rsgl *rsgl, *tmp;
	struct scatterlist *sgl;
	struct scatterlist *sg;
	int i, n;

	list_for_each_entry_safe(rsgl, tmp, &sreq->list, list) {
		af_alg_free_sg(&rsgl->sgl);
		if (rsgl != &sreq->first_sgl)
			kfree(rsgl);
	}
	sgl = sreq->tsg;
	n = sg_nents(sgl);
	for_each_sg(sgl, sg, n, i)
		put_page(sg_page(sg));

	kfree(sreq->tsg);
}