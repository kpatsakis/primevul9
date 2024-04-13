static void reloc_gpu_flush(struct reloc_cache *cache)
{
	GEM_BUG_ON(cache->rq_size >= cache->rq->batch->obj->base.size / sizeof(u32));
	cache->rq_cmd[cache->rq_size] = MI_BATCH_BUFFER_END;
	i915_gem_object_unpin_map(cache->rq->batch->obj);
	i915_gem_chipset_flush(cache->rq->i915);

	i915_request_add(cache->rq);
	cache->rq = NULL;
}