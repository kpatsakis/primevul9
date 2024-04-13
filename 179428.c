i915_gem_execbuffer_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *file)
{
	struct drm_i915_gem_execbuffer *args = data;
	struct drm_i915_gem_execbuffer2 exec2;
	struct drm_i915_gem_exec_object *exec_list = NULL;
	struct drm_i915_gem_exec_object2 *exec2_list = NULL;
	const size_t count = args->buffer_count;
	unsigned int i;
	int err;

	if (!check_buffer_count(count)) {
		DRM_DEBUG("execbuf2 with %zd buffers\n", count);
		return -EINVAL;
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

	if (!i915_gem_check_execbuffer(&exec2))
		return -EINVAL;

	/* Copy in the exec list from userland */
	exec_list = kvmalloc_array(count, sizeof(*exec_list),
				   __GFP_NOWARN | GFP_KERNEL);
	exec2_list = kvmalloc_array(count + 1, eb_element_size(),
				    __GFP_NOWARN | GFP_KERNEL);
	if (exec_list == NULL || exec2_list == NULL) {
		DRM_DEBUG("Failed to allocate exec list for %d buffers\n",
			  args->buffer_count);
		kvfree(exec_list);
		kvfree(exec2_list);
		return -ENOMEM;
	}
	err = copy_from_user(exec_list,
			     u64_to_user_ptr(args->buffers_ptr),
			     sizeof(*exec_list) * count);
	if (err) {
		DRM_DEBUG("copy %d exec entries failed %d\n",
			  args->buffer_count, err);
		kvfree(exec_list);
		kvfree(exec2_list);
		return -EFAULT;
	}

	for (i = 0; i < args->buffer_count; i++) {
		exec2_list[i].handle = exec_list[i].handle;
		exec2_list[i].relocation_count = exec_list[i].relocation_count;
		exec2_list[i].relocs_ptr = exec_list[i].relocs_ptr;
		exec2_list[i].alignment = exec_list[i].alignment;
		exec2_list[i].offset = exec_list[i].offset;
		if (INTEL_GEN(to_i915(dev)) < 4)
			exec2_list[i].flags = EXEC_OBJECT_NEEDS_FENCE;
		else
			exec2_list[i].flags = 0;
	}

	err = i915_gem_do_execbuffer(dev, file, &exec2, exec2_list, NULL);
	if (exec2.flags & __EXEC_HAS_RELOC) {
		struct drm_i915_gem_exec_object __user *user_exec_list =
			u64_to_user_ptr(args->buffers_ptr);

		/* Copy the new buffer offsets back to the user's exec list. */
		for (i = 0; i < args->buffer_count; i++) {
			if (!(exec2_list[i].offset & UPDATE))
				continue;

			exec2_list[i].offset =
				gen8_canonical_addr(exec2_list[i].offset & PIN_OFFSET_MASK);
			exec2_list[i].offset &= PIN_OFFSET_MASK;
			if (__copy_to_user(&user_exec_list[i].offset,
					   &exec2_list[i].offset,
					   sizeof(user_exec_list[i].offset)))
				break;
		}
	}

	kvfree(exec_list);
	kvfree(exec2_list);
	return err;
}