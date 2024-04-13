static int skcipher_all_sg_nents(struct skcipher_ctx *ctx)
{
	struct skcipher_sg_list *sgl;
	struct scatterlist *sg;
	int nents = 0;

	list_for_each_entry(sgl, &ctx->tsgl, list) {
		sg = sgl->sg;

		while (!sg->length)
			sg++;

		nents += sg_nents(sg);
	}
	return nents;
}