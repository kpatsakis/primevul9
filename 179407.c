eb_add_vma(struct i915_execbuffer *eb,
	   unsigned int i, unsigned batch_idx,
	   struct i915_vma *vma)
{
	struct drm_i915_gem_exec_object2 *entry = &eb->exec[i];
	int err;

	GEM_BUG_ON(i915_vma_is_closed(vma));

	if (!(eb->args->flags & __EXEC_VALIDATED)) {
		err = eb_validate_vma(eb, entry, vma);
		if (unlikely(err))
			return err;
	}

	if (eb->lut_size > 0) {
		vma->exec_handle = entry->handle;
		hlist_add_head(&vma->exec_node,
			       &eb->buckets[hash_32(entry->handle,
						    eb->lut_size)]);
	}

	if (entry->relocation_count)
		list_add_tail(&vma->reloc_link, &eb->relocs);

	/*
	 * Stash a pointer from the vma to execobj, so we can query its flags,
	 * size, alignment etc as provided by the user. Also we stash a pointer
	 * to the vma inside the execobj so that we can use a direct lookup
	 * to find the right target VMA when doing relocations.
	 */
	eb->vma[i] = vma;
	eb->flags[i] = entry->flags;
	vma->exec_flags = &eb->flags[i];

	/*
	 * SNA is doing fancy tricks with compressing batch buffers, which leads
	 * to negative relocation deltas. Usually that works out ok since the
	 * relocate address is still positive, except when the batch is placed
	 * very low in the GTT. Ensure this doesn't happen.
	 *
	 * Note that actual hangs have only been observed on gen7, but for
	 * paranoia do it everywhere.
	 */
	if (i == batch_idx) {
		if (entry->relocation_count &&
		    !(eb->flags[i] & EXEC_OBJECT_PINNED))
			eb->flags[i] |= __EXEC_OBJECT_NEEDS_BIAS;
		if (eb->reloc_cache.has_fence)
			eb->flags[i] |= EXEC_OBJECT_NEEDS_FENCE;

		eb->batch = vma;
	}

	err = 0;
	if (eb_pin_vma(eb, entry, vma)) {
		if (entry->offset != vma->node.start) {
			entry->offset = vma->node.start | UPDATE;
			eb->args->flags |= __EXEC_HAS_RELOC;
		}
	} else {
		eb_unreserve_vma(vma, vma->exec_flags);

		list_add_tail(&vma->exec_link, &eb->unbound);
		if (drm_mm_node_allocated(&vma->node))
			err = i915_vma_unbind(vma);
		if (unlikely(err))
			vma->exec_flags = NULL;
	}
	return err;
}