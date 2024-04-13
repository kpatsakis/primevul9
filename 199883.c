void snd_free_pages(void *ptr, size_t size)
{
	int pg;

	if (ptr == NULL)
		return;
	pg = get_order(size);
	dec_snd_pages(pg);
	free_pages((unsigned long) ptr, pg);
}