int af_alg_sendmsg(struct socket *sock, struct msghdr *msg, size_t size,
		   unsigned int ivsize)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct af_alg_tsgl *sgl;
	struct af_alg_control con = {};
	long copied = 0;
	bool enc = 0;
	bool init = 0;
	int err = 0;

	if (msg->msg_controllen) {
		err = af_alg_cmsg_send(msg, &con);
		if (err)
			return err;

		init = 1;
		switch (con.op) {
		case ALG_OP_ENCRYPT:
			enc = 1;
			break;
		case ALG_OP_DECRYPT:
			enc = 0;
			break;
		default:
			return -EINVAL;
		}

		if (con.iv && con.iv->ivlen != ivsize)
			return -EINVAL;
	}

	lock_sock(sk);
	if (!ctx->more && ctx->used) {
		err = -EINVAL;
		goto unlock;
	}

	if (init) {
		ctx->enc = enc;
		if (con.iv)
			memcpy(ctx->iv, con.iv->iv, ivsize);

		ctx->aead_assoclen = con.aead_assoclen;
	}

	while (size) {
		struct scatterlist *sg;
		size_t len = size;
		size_t plen;

		/* use the existing memory in an allocated page */
		if (ctx->merge) {
			sgl = list_entry(ctx->tsgl_list.prev,
					 struct af_alg_tsgl, list);
			sg = sgl->sg + sgl->cur - 1;
			len = min_t(size_t, len,
				    PAGE_SIZE - sg->offset - sg->length);

			err = memcpy_from_msg(page_address(sg_page(sg)) +
					      sg->offset + sg->length,
					      msg, len);
			if (err)
				goto unlock;

			sg->length += len;
			ctx->merge = (sg->offset + sg->length) &
				     (PAGE_SIZE - 1);

			ctx->used += len;
			copied += len;
			size -= len;
			continue;
		}

		if (!af_alg_writable(sk)) {
			err = af_alg_wait_for_wmem(sk, msg->msg_flags);
			if (err)
				goto unlock;
		}

		/* allocate a new page */
		len = min_t(unsigned long, len, af_alg_sndbuf(sk));

		err = af_alg_alloc_tsgl(sk);
		if (err)
			goto unlock;

		sgl = list_entry(ctx->tsgl_list.prev, struct af_alg_tsgl,
				 list);
		sg = sgl->sg;
		if (sgl->cur)
			sg_unmark_end(sg + sgl->cur - 1);

		do {
			unsigned int i = sgl->cur;

			plen = min_t(size_t, len, PAGE_SIZE);

			sg_assign_page(sg + i, alloc_page(GFP_KERNEL));
			if (!sg_page(sg + i)) {
				err = -ENOMEM;
				goto unlock;
			}

			err = memcpy_from_msg(page_address(sg_page(sg + i)),
					      msg, plen);
			if (err) {
				__free_page(sg_page(sg + i));
				sg_assign_page(sg + i, NULL);
				goto unlock;
			}

			sg[i].length = plen;
			len -= plen;
			ctx->used += plen;
			copied += plen;
			size -= plen;
			sgl->cur++;
		} while (len && sgl->cur < MAX_SGL_ENTS);

		if (!size)
			sg_mark_end(sg + sgl->cur - 1);

		ctx->merge = plen & (PAGE_SIZE - 1);
	}

	err = 0;

	ctx->more = msg->msg_flags & MSG_MORE;

unlock:
	af_alg_data_wakeup(sk);
	release_sock(sk);

	return copied ?: err;
}