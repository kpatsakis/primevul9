regional_alloc_zero(struct regional *r, size_t size)
{
	void *s = regional_alloc(r, size);
	if(!s) return NULL;
	memset(s, 0, size);
	return s;
}