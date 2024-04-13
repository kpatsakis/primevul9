static struct i915_vma *eb_parse(struct i915_execbuffer *eb, bool is_master)
{
	struct drm_i915_gem_object *shadow_batch_obj;
	struct i915_vma *vma;
	int err;

	shadow_batch_obj = i915_gem_batch_pool_get(&eb->engine->batch_pool,
						   PAGE_ALIGN(eb->batch_len));
	if (IS_ERR(shadow_batch_obj))
		return ERR_CAST(shadow_batch_obj);

	err = intel_engine_cmd_parser(eb->engine,
				      eb->batch->obj,
				      shadow_batch_obj,
				      eb->batch_start_offset,
				      eb->batch_len,
				      is_master);
	if (err) {
		if (err == -EACCES) /* unhandled chained batch */
			vma = NULL;
		else
			vma = ERR_PTR(err);
		goto out;
	}

	vma = i915_gem_object_ggtt_pin(shadow_batch_obj, NULL, 0, 0, 0);
	if (IS_ERR(vma))
		goto out;

	eb->vma[eb->buffer_count] = i915_vma_get(vma);
	eb->flags[eb->buffer_count] =
		__EXEC_OBJECT_HAS_PIN | __EXEC_OBJECT_HAS_REF;
	vma->exec_flags = &eb->flags[eb->buffer_count];
	eb->buffer_count++;

out:
	i915_gem_object_unpin_pages(shadow_batch_obj);
	return vma;
}