unsigned int af_alg_count_tsgl(struct sock *sk, size_t bytes, size_t offset)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct af_alg_tsgl *sgl, *tmp;
	unsigned int i;
	unsigned int sgl_count = 0;

	if (!bytes)
		return 0;

	list_for_each_entry_safe(sgl, tmp, &ctx->tsgl_list, list) {
		struct scatterlist *sg = sgl->sg;

		for (i = 0; i < sgl->cur; i++) {
			size_t bytes_count;

			/* Skip offset */
			if (offset >= sg[i].length) {
				offset -= sg[i].length;
				bytes -= sg[i].length;
				continue;
			}

			bytes_count = sg[i].length - offset;

			offset = 0;
			sgl_count++;

			/* If we have seen requested number of bytes, stop */
			if (bytes_count >= bytes)
				return sgl_count;

			bytes -= bytes_count;
		}
	}

	return sgl_count;
}