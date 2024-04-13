static int skcipher_recvmsg_async(struct socket *sock, struct msghdr *msg,
				  int flags)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct skcipher_sg_list *sgl;
	struct scatterlist *sg;
	struct skcipher_async_req *sreq;
	struct skcipher_request *req;
	struct skcipher_async_rsgl *last_rsgl = NULL;
	unsigned int txbufs = 0, len = 0, tx_nents = skcipher_all_sg_nents(ctx);
	unsigned int reqlen = sizeof(struct skcipher_async_req) +
				GET_REQ_SIZE(ctx) + GET_IV_SIZE(ctx);
	int err = -ENOMEM;
	bool mark = false;

	lock_sock(sk);
	req = kmalloc(reqlen, GFP_KERNEL);
	if (unlikely(!req))
		goto unlock;

	sreq = GET_SREQ(req, ctx);
	sreq->iocb = msg->msg_iocb;
	memset(&sreq->first_sgl, '\0', sizeof(struct skcipher_async_rsgl));
	INIT_LIST_HEAD(&sreq->list);
	sreq->tsg = kcalloc(tx_nents, sizeof(*sg), GFP_KERNEL);
	if (unlikely(!sreq->tsg)) {
		kfree(req);
		goto unlock;
	}
	sg_init_table(sreq->tsg, tx_nents);
	memcpy(sreq->iv, ctx->iv, GET_IV_SIZE(ctx));
	skcipher_request_set_tfm(req, crypto_skcipher_reqtfm(&ctx->req));
	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG,
				      skcipher_async_cb, sk);

	while (iov_iter_count(&msg->msg_iter)) {
		struct skcipher_async_rsgl *rsgl;
		int used;

		if (!ctx->used) {
			err = skcipher_wait_for_data(sk, flags);
			if (err)
				goto free;
		}
		sgl = list_first_entry(&ctx->tsgl,
				       struct skcipher_sg_list, list);
		sg = sgl->sg;

		while (!sg->length)
			sg++;

		used = min_t(unsigned long, ctx->used,
			     iov_iter_count(&msg->msg_iter));
		used = min_t(unsigned long, used, sg->length);

		if (txbufs == tx_nents) {
			struct scatterlist *tmp;
			int x;
			/* Ran out of tx slots in async request
			 * need to expand */
			tmp = kcalloc(tx_nents * 2, sizeof(*tmp),
				      GFP_KERNEL);
			if (!tmp)
				goto free;

			sg_init_table(tmp, tx_nents * 2);
			for (x = 0; x < tx_nents; x++)
				sg_set_page(&tmp[x], sg_page(&sreq->tsg[x]),
					    sreq->tsg[x].length,
					    sreq->tsg[x].offset);
			kfree(sreq->tsg);
			sreq->tsg = tmp;
			tx_nents *= 2;
			mark = true;
		}
		/* Need to take over the tx sgl from ctx
		 * to the asynch req - these sgls will be freed later */
		sg_set_page(sreq->tsg + txbufs++, sg_page(sg), sg->length,
			    sg->offset);

		if (list_empty(&sreq->list)) {
			rsgl = &sreq->first_sgl;
			list_add_tail(&rsgl->list, &sreq->list);
		} else {
			rsgl = kmalloc(sizeof(*rsgl), GFP_KERNEL);
			if (!rsgl) {
				err = -ENOMEM;
				goto free;
			}
			list_add_tail(&rsgl->list, &sreq->list);
		}

		used = af_alg_make_sg(&rsgl->sgl, &msg->msg_iter, used);
		err = used;
		if (used < 0)
			goto free;
		if (last_rsgl)
			af_alg_link_sg(&last_rsgl->sgl, &rsgl->sgl);

		last_rsgl = rsgl;
		len += used;
		skcipher_pull_sgl(sk, used, 0);
		iov_iter_advance(&msg->msg_iter, used);
	}

	if (mark)
		sg_mark_end(sreq->tsg + txbufs - 1);

	skcipher_request_set_crypt(req, sreq->tsg, sreq->first_sgl.sgl.sg,
				   len, sreq->iv);
	err = ctx->enc ? crypto_skcipher_encrypt(req) :
			 crypto_skcipher_decrypt(req);
	if (err == -EINPROGRESS) {
		atomic_inc(&ctx->inflight);
		err = -EIOCBQUEUED;
		goto unlock;
	}
free:
	skcipher_free_async_sgls(sreq);
	kfree(req);
unlock:
	skcipher_wmem_wakeup(sk);
	release_sock(sk);
	return err;
}