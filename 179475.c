static u32 *reloc_gpu(struct i915_execbuffer *eb,
		      struct i915_vma *vma,
		      unsigned int len)
{
	struct reloc_cache *cache = &eb->reloc_cache;
	u32 *cmd;

	if (cache->rq_size > PAGE_SIZE/sizeof(u32) - (len + 1))
		reloc_gpu_flush(cache);

	if (unlikely(!cache->rq)) {
		int err;

		/* If we need to copy for the cmdparser, we will stall anyway */
		if (eb_use_cmdparser(eb))
			return ERR_PTR(-EWOULDBLOCK);

		if (!intel_engine_can_store_dword(eb->engine))
			return ERR_PTR(-ENODEV);

		err = __reloc_gpu_alloc(eb, vma, len);
		if (unlikely(err))
			return ERR_PTR(err);
	}

	cmd = cache->rq_cmd + cache->rq_size;
	cache->rq_size += len;

	return cmd;
}