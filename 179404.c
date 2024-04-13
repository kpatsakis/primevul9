static int __reloc_gpu_alloc(struct i915_execbuffer *eb,
			     struct i915_vma *vma,
			     unsigned int len)
{
	struct reloc_cache *cache = &eb->reloc_cache;
	struct drm_i915_gem_object *obj;
	struct i915_request *rq;
	struct i915_vma *batch;
	u32 *cmd;
	int err;

	if (DBG_FORCE_RELOC == FORCE_GPU_RELOC) {
		obj = vma->obj;
		if (obj->cache_dirty & ~obj->cache_coherent)
			i915_gem_clflush_object(obj, 0);
		obj->write_domain = 0;
	}

	GEM_BUG_ON(vma->obj->write_domain & I915_GEM_DOMAIN_CPU);

	obj = i915_gem_batch_pool_get(&eb->engine->batch_pool, PAGE_SIZE);
	if (IS_ERR(obj))
		return PTR_ERR(obj);

	cmd = i915_gem_object_pin_map(obj,
				      cache->has_llc ?
				      I915_MAP_FORCE_WB :
				      I915_MAP_FORCE_WC);
	i915_gem_object_unpin_pages(obj);
	if (IS_ERR(cmd))
		return PTR_ERR(cmd);

	err = i915_gem_object_set_to_wc_domain(obj, false);
	if (err)
		goto err_unmap;

	batch = i915_vma_instance(obj, vma->vm, NULL);
	if (IS_ERR(batch)) {
		err = PTR_ERR(batch);
		goto err_unmap;
	}

	err = i915_vma_pin(batch, 0, 0, PIN_USER | PIN_NONBLOCK);
	if (err)
		goto err_unmap;

	rq = i915_request_alloc(eb->engine, eb->ctx);
	if (IS_ERR(rq)) {
		err = PTR_ERR(rq);
		goto err_unpin;
	}

	err = i915_request_await_object(rq, vma->obj, true);
	if (err)
		goto err_request;

	err = eb->engine->emit_bb_start(rq,
					batch->node.start, PAGE_SIZE,
					cache->gen > 5 ? 0 : I915_DISPATCH_SECURE);
	if (err)
		goto err_request;

	GEM_BUG_ON(!reservation_object_test_signaled_rcu(batch->resv, true));
	err = i915_vma_move_to_active(batch, rq, 0);
	if (err)
		goto skip_request;

	err = i915_vma_move_to_active(vma, rq, EXEC_OBJECT_WRITE);
	if (err)
		goto skip_request;

	rq->batch = batch;
	i915_vma_unpin(batch);

	cache->rq = rq;
	cache->rq_cmd = cmd;
	cache->rq_size = 0;

	/* Return with batch mapping (cmd) still pinned */
	return 0;

skip_request:
	i915_request_skip(rq, err);
err_request:
	i915_request_add(rq);
err_unpin:
	i915_vma_unpin(batch);
err_unmap:
	i915_gem_object_unpin_map(obj);
	return err;
}