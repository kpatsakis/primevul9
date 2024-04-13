regional_alloc_init(struct regional* r, const void *init, size_t size)
{
	void *s = regional_alloc(r, size);
	if(!s) return NULL;
	memcpy(s, init, size);
	return s;
}