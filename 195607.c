void *ext4_kvmalloc(size_t size, gfp_t flags)
{
	void *ret;

	ret = kmalloc(size, flags);
	if (!ret)
		ret = __vmalloc(size, flags, PAGE_KERNEL);
	return ret;
}