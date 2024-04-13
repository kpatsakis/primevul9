ssize_t af_alg_sendpage(struct socket *sock, struct page *page,
			int offset, size_t size, int flags)
{
	struct sock *sk = sock->sk;
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct af_alg_tsgl *sgl;
	int err = -EINVAL;

	if (flags & MSG_SENDPAGE_NOTLAST)
		flags |= MSG_MORE;

	lock_sock(sk);
	if (!ctx->more && ctx->used)
		goto unlock;

	if (!size)
		goto done;

	if (!af_alg_writable(sk)) {
		err = af_alg_wait_for_wmem(sk, flags);
		if (err)
			goto unlock;
	}

	err = af_alg_alloc_tsgl(sk);
	if (err)
		goto unlock;

	ctx->merge = 0;
	sgl = list_entry(ctx->tsgl_list.prev, struct af_alg_tsgl, list);

	if (sgl->cur)
		sg_unmark_end(sgl->sg + sgl->cur - 1);

	sg_mark_end(sgl->sg + sgl->cur);

	get_page(page);
	sg_set_page(sgl->sg + sgl->cur, page, size, offset);
	sgl->cur++;
	ctx->used += size;

done:
	ctx->more = flags & MSG_MORE;

unlock:
	af_alg_data_wakeup(sk);
	release_sock(sk);

	return err ?: size;
}