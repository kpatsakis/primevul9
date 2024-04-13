void af_alg_link_sg(struct af_alg_sgl *sgl_prev, struct af_alg_sgl *sgl_new)
{
	sg_unmark_end(sgl_prev->sg + sgl_prev->npages - 1);
	sg_chain(sgl_prev->sg, sgl_prev->npages + 1, sgl_new->sg);
}