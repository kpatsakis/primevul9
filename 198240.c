i915_gem_execbuffer_relocate_entry(struct drm_i915_gem_object *obj,
				   struct eb_objects *eb,
				   struct drm_i915_gem_relocation_entry *reloc)
{
	struct drm_device *dev = obj->base.dev;
	struct drm_gem_object *target_obj;
	struct drm_i915_gem_object *target_i915_obj;
	uint32_t target_offset;
	int ret = -EINVAL;

	/* we've already hold a reference to all valid objects */
	target_obj = &eb_get_object(eb, reloc->target_handle)->base;
	if (unlikely(target_obj == NULL))
		return -ENOENT;

	target_i915_obj = to_intel_bo(target_obj);
	target_offset = target_i915_obj->gtt_offset;

	/* Sandybridge PPGTT errata: We need a global gtt mapping for MI and
	 * pipe_control writes because the gpu doesn't properly redirect them
	 * through the ppgtt for non_secure batchbuffers. */
	if (unlikely(IS_GEN6(dev) &&
	    reloc->write_domain == I915_GEM_DOMAIN_INSTRUCTION &&
	    !target_i915_obj->has_global_gtt_mapping)) {
		i915_gem_gtt_bind_object(target_i915_obj,
					 target_i915_obj->cache_level);
	}

	/* Validate that the target is in a valid r/w GPU domain */
	if (unlikely(reloc->write_domain & (reloc->write_domain - 1))) {
		DRM_DEBUG("reloc with multiple write domains: "
			  "obj %p target %d offset %d "
			  "read %08x write %08x",
			  obj, reloc->target_handle,
			  (int) reloc->offset,
			  reloc->read_domains,
			  reloc->write_domain);
		return ret;
	}
	if (unlikely((reloc->write_domain | reloc->read_domains)
		     & ~I915_GEM_GPU_DOMAINS)) {
		DRM_DEBUG("reloc with read/write non-GPU domains: "
			  "obj %p target %d offset %d "
			  "read %08x write %08x",
			  obj, reloc->target_handle,
			  (int) reloc->offset,
			  reloc->read_domains,
			  reloc->write_domain);
		return ret;
	}

	target_obj->pending_read_domains |= reloc->read_domains;
	target_obj->pending_write_domain |= reloc->write_domain;

	/* If the relocation already has the right value in it, no
	 * more work needs to be done.
	 */
	if (target_offset == reloc->presumed_offset)
		return 0;

	/* Check that the relocation address is valid... */
	if (unlikely(reloc->offset > obj->base.size - 4)) {
		DRM_DEBUG("Relocation beyond object bounds: "
			  "obj %p target %d offset %d size %d.\n",
			  obj, reloc->target_handle,
			  (int) reloc->offset,
			  (int) obj->base.size);
		return ret;
	}
	if (unlikely(reloc->offset & 3)) {
		DRM_DEBUG("Relocation not 4-byte aligned: "
			  "obj %p target %d offset %d.\n",
			  obj, reloc->target_handle,
			  (int) reloc->offset);
		return ret;
	}

	/* We can't wait for rendering with pagefaults disabled */
	if (obj->active && in_atomic())
		return -EFAULT;

	reloc->delta += target_offset;
	if (use_cpu_reloc(obj)) {
		uint32_t page_offset = reloc->offset & ~PAGE_MASK;
		char *vaddr;

		ret = i915_gem_object_set_to_cpu_domain(obj, 1);
		if (ret)
			return ret;

		vaddr = kmap_atomic(i915_gem_object_get_page(obj,
							     reloc->offset >> PAGE_SHIFT));
		*(uint32_t *)(vaddr + page_offset) = reloc->delta;
		kunmap_atomic(vaddr);
	} else {
		struct drm_i915_private *dev_priv = dev->dev_private;
		uint32_t __iomem *reloc_entry;
		void __iomem *reloc_page;

		ret = i915_gem_object_set_to_gtt_domain(obj, true);
		if (ret)
			return ret;

		ret = i915_gem_object_put_fence(obj);
		if (ret)
			return ret;

		/* Map the page containing the relocation we're going to perform.  */
		reloc->offset += obj->gtt_offset;
		reloc_page = io_mapping_map_atomic_wc(dev_priv->gtt.mappable,
						      reloc->offset & PAGE_MASK);
		reloc_entry = (uint32_t __iomem *)
			(reloc_page + (reloc->offset & ~PAGE_MASK));
		iowrite32(reloc->delta, reloc_entry);
		io_mapping_unmap_atomic(reloc_page);
	}

	/* and update the user's relocation entry */
	reloc->presumed_offset = target_offset;

	return 0;
}