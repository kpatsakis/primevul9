void af_alg_free_sg(struct af_alg_sgl *sgl)
{
	int i;

	for (i = 0; i < sgl->npages; i++)
		put_page(sgl->pages[i]);
}