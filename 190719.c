static inline void *f2fs_kmem_cache_alloc(struct kmem_cache *cachep,
						gfp_t flags)
{
	void *entry;

	entry = kmem_cache_alloc(cachep, flags);
	if (!entry)
		entry = kmem_cache_alloc(cachep, flags | __GFP_NOFAIL);
	return entry;
}