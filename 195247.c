static void dsfree_pages(const void *buffer, size_t size)
{
	if (buffer)
		free_pages((unsigned long)buffer, get_order(size));
}