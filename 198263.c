i915_gem_do_execbuffer(struct drm_device *dev, void *data,
		       struct drm_file *file,
		       struct drm_i915_gem_execbuffer2 *args,
		       struct drm_i915_gem_exec_object2 *exec)
{
	drm_i915_private_t *dev_priv = dev->dev_private;
	struct eb_objects *eb;
	struct drm_i915_gem_object *batch_obj;
	struct drm_clip_rect *cliprects = NULL;
	struct intel_ring_buffer *ring;
	u32 ctx_id = i915_execbuffer2_get_context_id(*args);
	u32 exec_start, exec_len;
	u32 mask, flags;
	int ret, mode, i;
	bool need_relocs;

	if (!i915_gem_check_execbuffer(args))
		return -EINVAL;

	ret = validate_exec_list(exec, args->buffer_count);
	if (ret)
		return ret;

	flags = 0;
	if (args->flags & I915_EXEC_SECURE) {
		if (!file->is_master || !capable(CAP_SYS_ADMIN))
		    return -EPERM;

		flags |= I915_DISPATCH_SECURE;
	}
	if (args->flags & I915_EXEC_IS_PINNED)
		flags |= I915_DISPATCH_PINNED;

	switch (args->flags & I915_EXEC_RING_MASK) {
	case I915_EXEC_DEFAULT:
	case I915_EXEC_RENDER:
		ring = &dev_priv->ring[RCS];
		break;
	case I915_EXEC_BSD:
		ring = &dev_priv->ring[VCS];
		if (ctx_id != 0) {
			DRM_DEBUG("Ring %s doesn't support contexts\n",
				  ring->name);
			return -EPERM;
		}
		break;
	case I915_EXEC_BLT:
		ring = &dev_priv->ring[BCS];
		if (ctx_id != 0) {
			DRM_DEBUG("Ring %s doesn't support contexts\n",
				  ring->name);
			return -EPERM;
		}
		break;
	default:
		DRM_DEBUG("execbuf with unknown ring: %d\n",
			  (int)(args->flags & I915_EXEC_RING_MASK));
		return -EINVAL;
	}
	if (!intel_ring_initialized(ring)) {
		DRM_DEBUG("execbuf with invalid ring: %d\n",
			  (int)(args->flags & I915_EXEC_RING_MASK));
		return -EINVAL;
	}

	mode = args->flags & I915_EXEC_CONSTANTS_MASK;
	mask = I915_EXEC_CONSTANTS_MASK;
	switch (mode) {
	case I915_EXEC_CONSTANTS_REL_GENERAL:
	case I915_EXEC_CONSTANTS_ABSOLUTE:
	case I915_EXEC_CONSTANTS_REL_SURFACE:
		if (ring == &dev_priv->ring[RCS] &&
		    mode != dev_priv->relative_constants_mode) {
			if (INTEL_INFO(dev)->gen < 4)
				return -EINVAL;

			if (INTEL_INFO(dev)->gen > 5 &&
			    mode == I915_EXEC_CONSTANTS_REL_SURFACE)
				return -EINVAL;

			/* The HW changed the meaning on this bit on gen6 */
			if (INTEL_INFO(dev)->gen >= 6)
				mask &= ~I915_EXEC_CONSTANTS_REL_SURFACE;
		}
		break;
	default:
		DRM_DEBUG("execbuf with unknown constants: %d\n", mode);
		return -EINVAL;
	}

	if (args->buffer_count < 1) {
		DRM_DEBUG("execbuf with %d buffers\n", args->buffer_count);
		return -EINVAL;
	}

	if (args->num_cliprects != 0) {
		if (ring != &dev_priv->ring[RCS]) {
			DRM_DEBUG("clip rectangles are only valid with the render ring\n");
			return -EINVAL;
		}

		if (INTEL_INFO(dev)->gen >= 5) {
			DRM_DEBUG("clip rectangles are only valid on pre-gen5\n");
			return -EINVAL;
		}

		if (args->num_cliprects > UINT_MAX / sizeof(*cliprects)) {
			DRM_DEBUG("execbuf with %u cliprects\n",
				  args->num_cliprects);
			return -EINVAL;
		}

		cliprects = kmalloc(args->num_cliprects * sizeof(*cliprects),
				    GFP_KERNEL);
		if (cliprects == NULL) {
			ret = -ENOMEM;
			goto pre_mutex_err;
		}

		if (copy_from_user(cliprects,
				     (struct drm_clip_rect __user *)(uintptr_t)
				     args->cliprects_ptr,
				     sizeof(*cliprects)*args->num_cliprects)) {
			ret = -EFAULT;
			goto pre_mutex_err;
		}
	}

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto pre_mutex_err;

	if (dev_priv->mm.suspended) {
		mutex_unlock(&dev->struct_mutex);
		ret = -EBUSY;
		goto pre_mutex_err;
	}

	eb = eb_create(args);
	if (eb == NULL) {
		mutex_unlock(&dev->struct_mutex);
		ret = -ENOMEM;
		goto pre_mutex_err;
	}

	/* Look up object handles */
	ret = eb_lookup_objects(eb, exec, args, file);
	if (ret)
		goto err;

	/* take note of the batch buffer before we might reorder the lists */
	batch_obj = list_entry(eb->objects.prev,
			       struct drm_i915_gem_object,
			       exec_list);

	/* Move the objects en-masse into the GTT, evicting if necessary. */
	need_relocs = (args->flags & I915_EXEC_NO_RELOC) == 0;
	ret = i915_gem_execbuffer_reserve(ring, file, &eb->objects, &need_relocs);
	if (ret)
		goto err;

	/* The objects are in their final locations, apply the relocations. */
	if (need_relocs)
		ret = i915_gem_execbuffer_relocate(dev, eb);
	if (ret) {
		if (ret == -EFAULT) {
			ret = i915_gem_execbuffer_relocate_slow(dev, args, file, ring,
								eb, exec);
			BUG_ON(!mutex_is_locked(&dev->struct_mutex));
		}
		if (ret)
			goto err;
	}

	/* Set the pending read domains for the batch buffer to COMMAND */
	if (batch_obj->base.pending_write_domain) {
		DRM_DEBUG("Attempting to use self-modifying batch buffer\n");
		ret = -EINVAL;
		goto err;
	}
	batch_obj->base.pending_read_domains |= I915_GEM_DOMAIN_COMMAND;

	/* snb/ivb/vlv conflate the "batch in ppgtt" bit with the "non-secure
	 * batch" bit. Hence we need to pin secure batches into the global gtt.
	 * hsw should have this fixed, but let's be paranoid and do it
	 * unconditionally for now. */
	if (flags & I915_DISPATCH_SECURE && !batch_obj->has_global_gtt_mapping)
		i915_gem_gtt_bind_object(batch_obj, batch_obj->cache_level);

	ret = i915_gem_execbuffer_move_to_gpu(ring, &eb->objects);
	if (ret)
		goto err;

	ret = i915_switch_context(ring, file, ctx_id);
	if (ret)
		goto err;

	if (ring == &dev_priv->ring[RCS] &&
	    mode != dev_priv->relative_constants_mode) {
		ret = intel_ring_begin(ring, 4);
		if (ret)
				goto err;

		intel_ring_emit(ring, MI_NOOP);
		intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(1));
		intel_ring_emit(ring, INSTPM);
		intel_ring_emit(ring, mask << 16 | mode);
		intel_ring_advance(ring);

		dev_priv->relative_constants_mode = mode;
	}

	if (args->flags & I915_EXEC_GEN7_SOL_RESET) {
		ret = i915_reset_gen7_sol_offsets(dev, ring);
		if (ret)
			goto err;
	}

	exec_start = batch_obj->gtt_offset + args->batch_start_offset;
	exec_len = args->batch_len;
	if (cliprects) {
		for (i = 0; i < args->num_cliprects; i++) {
			ret = i915_emit_box(dev, &cliprects[i],
					    args->DR1, args->DR4);
			if (ret)
				goto err;

			ret = ring->dispatch_execbuffer(ring,
							exec_start, exec_len,
							flags);
			if (ret)
				goto err;
		}
	} else {
		ret = ring->dispatch_execbuffer(ring,
						exec_start, exec_len,
						flags);
		if (ret)
			goto err;
	}

	trace_i915_gem_ring_dispatch(ring, intel_ring_get_seqno(ring), flags);

	i915_gem_execbuffer_move_to_active(&eb->objects, ring);
	i915_gem_execbuffer_retire_commands(dev, file, ring);

err:
	eb_destroy(eb);

	mutex_unlock(&dev->struct_mutex);

pre_mutex_err:
	kfree(cliprects);
	return ret;
}