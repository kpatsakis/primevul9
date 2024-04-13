static struct page **get_pages_array(size_t n)
{
	return kvmalloc_array(n, sizeof(struct page *), GFP_KERNEL);
}