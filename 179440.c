get_fence_array(struct drm_i915_gem_execbuffer2 *args,
		struct drm_file *file)
{
	const unsigned long nfences = args->num_cliprects;
	struct drm_i915_gem_exec_fence __user *user;
	struct drm_syncobj **fences;
	unsigned long n;
	int err;

	if (!(args->flags & I915_EXEC_FENCE_ARRAY))
		return NULL;

	/* Check multiplication overflow for access_ok() and kvmalloc_array() */
	BUILD_BUG_ON(sizeof(size_t) > sizeof(unsigned long));
	if (nfences > min_t(unsigned long,
			    ULONG_MAX / sizeof(*user),
			    SIZE_MAX / sizeof(*fences)))
		return ERR_PTR(-EINVAL);

	user = u64_to_user_ptr(args->cliprects_ptr);
	if (!access_ok(user, nfences * sizeof(*user)))
		return ERR_PTR(-EFAULT);

	fences = kvmalloc_array(nfences, sizeof(*fences),
				__GFP_NOWARN | GFP_KERNEL);
	if (!fences)
		return ERR_PTR(-ENOMEM);

	for (n = 0; n < nfences; n++) {
		struct drm_i915_gem_exec_fence fence;
		struct drm_syncobj *syncobj;

		if (__copy_from_user(&fence, user++, sizeof(fence))) {
			err = -EFAULT;
			goto err;
		}

		if (fence.flags & __I915_EXEC_FENCE_UNKNOWN_FLAGS) {
			err = -EINVAL;
			goto err;
		}

		syncobj = drm_syncobj_find(file, fence.handle);
		if (!syncobj) {
			DRM_DEBUG("Invalid syncobj handle provided\n");
			err = -ENOENT;
			goto err;
		}

		BUILD_BUG_ON(~(ARCH_KMALLOC_MINALIGN - 1) &
			     ~__I915_EXEC_FENCE_UNKNOWN_FLAGS);

		fences[n] = ptr_pack_bits(syncobj, fence.flags, 2);
	}

	return fences;

err:
	__free_fence_array(fences, n);
	return ERR_PTR(err);
}