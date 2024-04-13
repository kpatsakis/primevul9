int af_alg_alloc_tsgl(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct af_alg_tsgl *sgl;
	struct scatterlist *sg = NULL;

	sgl = list_entry(ctx->tsgl_list.prev, struct af_alg_tsgl, list);
	if (!list_empty(&ctx->tsgl_list))
		sg = sgl->sg;

	if (!sg || sgl->cur >= MAX_SGL_ENTS) {
		sgl = sock_kmalloc(sk,
				   struct_size(sgl, sg, (MAX_SGL_ENTS + 1)),
				   GFP_KERNEL);
		if (!sgl)
			return -ENOMEM;

		sg_init_table(sgl->sg, MAX_SGL_ENTS + 1);
		sgl->cur = 0;

		if (sg)
			sg_chain(sg, MAX_SGL_ENTS + 1, sgl->sg);

		list_add_tail(&sgl->list, &ctx->tsgl_list);
	}

	return 0;
}