struct af_alg_async_req *af_alg_alloc_areq(struct sock *sk,
					   unsigned int areqlen)
{
	struct af_alg_async_req *areq = sock_kmalloc(sk, areqlen, GFP_KERNEL);

	if (unlikely(!areq))
		return ERR_PTR(-ENOMEM);

	areq->areqlen = areqlen;
	areq->sk = sk;
	areq->last_rsgl = NULL;
	INIT_LIST_HEAD(&areq->rsgl_list);
	areq->tsgl = NULL;
	areq->tsgl_entries = 0;

	return areq;
}