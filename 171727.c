void af_alg_pull_tsgl(struct sock *sk, size_t used, struct scatterlist *dst,
		      size_t dst_offset)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct af_alg_tsgl *sgl;
	struct scatterlist *sg;
	unsigned int i, j = 0;

	while (!list_empty(&ctx->tsgl_list)) {
		sgl = list_first_entry(&ctx->tsgl_list, struct af_alg_tsgl,
				       list);
		sg = sgl->sg;

		for (i = 0; i < sgl->cur; i++) {
			size_t plen = min_t(size_t, used, sg[i].length);
			struct page *page = sg_page(sg + i);

			if (!page)
				continue;

			/*
			 * Assumption: caller created af_alg_count_tsgl(len)
			 * SG entries in dst.
			 */
			if (dst) {
				if (dst_offset >= plen) {
					/* discard page before offset */
					dst_offset -= plen;
				} else {
					/* reassign page to dst after offset */
					get_page(page);
					sg_set_page(dst + j, page,
						    plen - dst_offset,
						    sg[i].offset + dst_offset);
					dst_offset = 0;
					j++;
				}
			}

			sg[i].length -= plen;
			sg[i].offset += plen;

			used -= plen;
			ctx->used -= plen;

			if (sg[i].length)
				return;

			put_page(page);
			sg_assign_page(sg + i, NULL);
		}

		list_del(&sgl->list);
		sock_kfree_s(sk, sgl, sizeof(*sgl) + sizeof(sgl->sg[0]) *
						     (MAX_SGL_ENTS + 1));
	}

	if (!ctx->used)
		ctx->merge = 0;
}