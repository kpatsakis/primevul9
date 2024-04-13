void af_alg_free_resources(struct af_alg_async_req *areq)
{
	struct sock *sk = areq->sk;

	af_alg_free_areq_sgls(areq);
	sock_kfree_s(sk, areq, areq->areqlen);
}