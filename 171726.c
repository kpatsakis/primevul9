int af_alg_get_rsgl(struct sock *sk, struct msghdr *msg, int flags,
		    struct af_alg_async_req *areq, size_t maxsize,
		    size_t *outlen)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	size_t len = 0;

	while (maxsize > len && msg_data_left(msg)) {
		struct af_alg_rsgl *rsgl;
		size_t seglen;
		int err;

		/* limit the amount of readable buffers */
		if (!af_alg_readable(sk))
			break;

		seglen = min_t(size_t, (maxsize - len),
			       msg_data_left(msg));

		if (list_empty(&areq->rsgl_list)) {
			rsgl = &areq->first_rsgl;
		} else {
			rsgl = sock_kmalloc(sk, sizeof(*rsgl), GFP_KERNEL);
			if (unlikely(!rsgl))
				return -ENOMEM;
		}

		rsgl->sgl.npages = 0;
		list_add_tail(&rsgl->list, &areq->rsgl_list);

		/* make one iovec available as scatterlist */
		err = af_alg_make_sg(&rsgl->sgl, &msg->msg_iter, seglen);
		if (err < 0) {
			rsgl->sg_num_bytes = 0;
			return err;
		}

		/* chain the new scatterlist with previous one */
		if (areq->last_rsgl)
			af_alg_link_sg(&areq->last_rsgl->sgl, &rsgl->sgl);

		areq->last_rsgl = rsgl;
		len += err;
		atomic_add(err, &ctx->rcvused);
		rsgl->sg_num_bytes = err;
		iov_iter_advance(&msg->msg_iter, err);
	}

	*outlen = len;
	return 0;
}