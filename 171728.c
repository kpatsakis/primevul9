void af_alg_async_cb(struct crypto_async_request *_req, int err)
{
	struct af_alg_async_req *areq = _req->data;
	struct sock *sk = areq->sk;
	struct kiocb *iocb = areq->iocb;
	unsigned int resultlen;

	/* Buffer size written by crypto operation. */
	resultlen = areq->outlen;

	af_alg_free_resources(areq);
	sock_put(sk);

	iocb->ki_complete(iocb, err ? err : resultlen, 0);
}