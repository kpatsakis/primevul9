static int skcipher_sendmsg(struct socket *sock, struct msghdr *msg,
			    size_t size)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(&ctx->req);
	unsigned ivsize = crypto_skcipher_ivsize(tfm);
	struct skcipher_sg_list *sgl;
	struct af_alg_control con = {};
	long copied = 0;
	bool enc = 0;
	bool init = 0;
	int err;
	int i;

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

	err = -EINVAL;

	lock_sock(sk);
	if (!ctx->more && ctx->used)
		goto unlock;

	if (init) {
		ctx->enc = enc;
		if (con.iv)
			memcpy(ctx->iv, con.iv->iv, ivsize);
	}

	while (size) {
		struct scatterlist *sg;
		unsigned long len = size;
		size_t plen;

		if (ctx->merge) {
			sgl = list_entry(ctx->tsgl.prev,
					 struct skcipher_sg_list, list);
			sg = sgl->sg + sgl->cur - 1;
			len = min_t(unsigned long, len,
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

		if (!skcipher_writable(sk)) {
			err = skcipher_wait_for_wmem(sk, msg->msg_flags);
			if (err)
				goto unlock;
		}

		len = min_t(unsigned long, len, skcipher_sndbuf(sk));

		err = skcipher_alloc_sgl(sk);
		if (err)
			goto unlock;

		sgl = list_entry(ctx->tsgl.prev, struct skcipher_sg_list, list);
		sg = sgl->sg;
		sg_unmark_end(sg + sgl->cur);
		do {
			i = sgl->cur;
			plen = min_t(size_t, len, PAGE_SIZE);

			sg_assign_page(sg + i, alloc_page(GFP_KERNEL));
			err = -ENOMEM;
			if (!sg_page(sg + i))
				goto unlock;

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
	skcipher_data_wakeup(sk);
	release_sock(sk);

	return copied ?: err;
}