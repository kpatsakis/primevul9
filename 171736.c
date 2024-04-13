void af_alg_free_areq_sgls(struct af_alg_async_req *areq)
{
	struct sock *sk = areq->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct af_alg_rsgl *rsgl, *tmp;
	struct scatterlist *tsgl;
	struct scatterlist *sg;
	unsigned int i;

	list_for_each_entry_safe(rsgl, tmp, &areq->rsgl_list, list) {
		atomic_sub(rsgl->sg_num_bytes, &ctx->rcvused);
		af_alg_free_sg(&rsgl->sgl);
		list_del(&rsgl->list);
		if (rsgl != &areq->first_rsgl)
			sock_kfree_s(sk, rsgl, sizeof(*rsgl));
	}

	tsgl = areq->tsgl;
	if (tsgl) {
		for_each_sg(tsgl, sg, areq->tsgl_entries, i) {
			if (!sg_page(sg))
				continue;
			put_page(sg_page(sg));
		}

		sock_kfree_s(sk, tsgl, areq->tsgl_entries * sizeof(*tsgl));
	}
}