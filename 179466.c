static int eb_lookup_vmas(struct i915_execbuffer *eb)
{
	struct radix_tree_root *handles_vma = &eb->ctx->handles_vma;
	struct drm_i915_gem_object *obj;
	unsigned int i, batch;
	int err;

	if (unlikely(i915_gem_context_is_closed(eb->ctx)))
		return -ENOENT;

	if (unlikely(i915_gem_context_is_banned(eb->ctx)))
		return -EIO;

	INIT_LIST_HEAD(&eb->relocs);
	INIT_LIST_HEAD(&eb->unbound);

	batch = eb_batch_index(eb);

	for (i = 0; i < eb->buffer_count; i++) {
		u32 handle = eb->exec[i].handle;
		struct i915_lut_handle *lut;
		struct i915_vma *vma;

		vma = radix_tree_lookup(handles_vma, handle);
		if (likely(vma))
			goto add_vma;

		obj = i915_gem_object_lookup(eb->file, handle);
		if (unlikely(!obj)) {
			err = -ENOENT;
			goto err_vma;
		}

		vma = i915_vma_instance(obj, eb->vm, NULL);
		if (unlikely(IS_ERR(vma))) {
			err = PTR_ERR(vma);
			goto err_obj;
		}

		lut = kmem_cache_alloc(eb->i915->luts, GFP_KERNEL);
		if (unlikely(!lut)) {
			err = -ENOMEM;
			goto err_obj;
		}

		err = radix_tree_insert(handles_vma, handle, vma);
		if (unlikely(err)) {
			kmem_cache_free(eb->i915->luts, lut);
			goto err_obj;
		}

		/* transfer ref to ctx */
		if (!vma->open_count++)
			i915_vma_reopen(vma);
		list_add(&lut->obj_link, &obj->lut_list);
		list_add(&lut->ctx_link, &eb->ctx->handles_list);
		lut->ctx = eb->ctx;
		lut->handle = handle;

add_vma:
		err = eb_add_vma(eb, i, batch, vma);
		if (unlikely(err))
			goto err_vma;

		GEM_BUG_ON(vma != eb->vma[i]);
		GEM_BUG_ON(vma->exec_flags != &eb->flags[i]);
		GEM_BUG_ON(drm_mm_node_allocated(&vma->node) &&
			   eb_vma_misplaced(&eb->exec[i], vma, eb->flags[i]));
	}

	eb->args->flags |= __EXEC_VALIDATED;
	return eb_reserve(eb);

err_obj:
	i915_gem_object_put(obj);
err_vma:
	eb->vma[i] = NULL;
	return err;
}