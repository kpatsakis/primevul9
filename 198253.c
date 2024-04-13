i915_gem_execbuffer(struct drm_device *dev, void *data,
		    struct drm_file *file)
{
	struct drm_i915_gem_execbuffer *args = data;
	struct drm_i915_gem_execbuffer2 exec2;
	struct drm_i915_gem_exec_object *exec_list = NULL;
	struct drm_i915_gem_exec_object2 *exec2_list = NULL;
	int ret, i;

	if (args->buffer_count < 1) {
		DRM_DEBUG("execbuf with %d buffers\n", args->buffer_count);
		return -EINVAL;
	}

	/* Copy in the exec list from userland */
	exec_list = drm_malloc_ab(sizeof(*exec_list), args->buffer_count);
	exec2_list = drm_malloc_ab(sizeof(*exec2_list), args->buffer_count);
	if (exec_list == NULL || exec2_list == NULL) {
		DRM_DEBUG("Failed to allocate exec list for %d buffers\n",
			  args->buffer_count);
		drm_free_large(exec_list);
		drm_free_large(exec2_list);
		return -ENOMEM;
	}
	ret = copy_from_user(exec_list,
			     (void __user *)(uintptr_t)args->buffers_ptr,
			     sizeof(*exec_list) * args->buffer_count);
	if (ret != 0) {
		DRM_DEBUG("copy %d exec entries failed %d\n",
			  args->buffer_count, ret);
		drm_free_large(exec_list);
		drm_free_large(exec2_list);
		return -EFAULT;
	}

	for (i = 0; i < args->buffer_count; i++) {
		exec2_list[i].handle = exec_list[i].handle;
		exec2_list[i].relocation_count = exec_list[i].relocation_count;
		exec2_list[i].relocs_ptr = exec_list[i].relocs_ptr;
		exec2_list[i].alignment = exec_list[i].alignment;
		exec2_list[i].offset = exec_list[i].offset;
		if (INTEL_INFO(dev)->gen < 4)
			exec2_list[i].flags = EXEC_OBJECT_NEEDS_FENCE;
		else
			exec2_list[i].flags = 0;
	}

	exec2.buffers_ptr = args->buffers_ptr;
	exec2.buffer_count = args->buffer_count;
	exec2.batch_start_offset = args->batch_start_offset;
	exec2.batch_len = args->batch_len;
	exec2.DR1 = args->DR1;
	exec2.DR4 = args->DR4;
	exec2.num_cliprects = args->num_cliprects;
	exec2.cliprects_ptr = args->cliprects_ptr;
	exec2.flags = I915_EXEC_RENDER;
	i915_execbuffer2_set_context_id(exec2, 0);

	ret = i915_gem_do_execbuffer(dev, data, file, &exec2, exec2_list);
	if (!ret) {
		/* Copy the new buffer offsets back to the user's exec list. */
		for (i = 0; i < args->buffer_count; i++)
			exec_list[i].offset = exec2_list[i].offset;
		/* ... and back out to userspace */
		ret = copy_to_user((void __user *)(uintptr_t)args->buffers_ptr,
				   exec_list,
				   sizeof(*exec_list) * args->buffer_count);
		if (ret) {
			ret = -EFAULT;
			DRM_DEBUG("failed to copy %d exec entries "
				  "back to user (%d)\n",
				  args->buffer_count, ret);
		}
	}

	drm_free_large(exec_list);
	drm_free_large(exec2_list);
	return ret;
}