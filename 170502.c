static void skcipher_pull_sgl(struct sock *sk, size_t used, int put)
{
	struct alg_sock *ask = alg_sk(sk);
	struct skcipher_ctx *ctx = ask->private;
	struct skcipher_sg_list *sgl;
	struct scatterlist *sg;
	int i;

	while (!list_empty(&ctx->tsgl)) {
		sgl = list_first_entry(&ctx->tsgl, struct skcipher_sg_list,
				       list);
		sg = sgl->sg;

		for (i = 0; i < sgl->cur; i++) {
			size_t plen = min_t(size_t, used, sg[i].length);

			if (!sg_page(sg + i))
				continue;

			sg[i].length -= plen;
			sg[i].offset += plen;

			used -= plen;
			ctx->used -= plen;

			if (sg[i].length)
				return;
			if (put)
				put_page(sg_page(sg + i));
			sg_assign_page(sg + i, NULL);
		}

		list_del(&sgl->list);
		sock_kfree_s(sk, sgl,
			     sizeof(*sgl) + sizeof(sgl->sg[0]) *
					    (MAX_SGL_ENTS + 1));
	}

	if (!ctx->used)
		ctx->merge = 0;
}