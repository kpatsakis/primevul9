i915_gem_userptr_ioctl(struct drm_device *dev,
		       void *data,
		       struct drm_file *file)
{
	static struct lock_class_key lock_class;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_i915_gem_userptr *args = data;
	struct drm_i915_gem_object *obj;
	int ret;
	u32 handle;

	if (!HAS_LLC(dev_priv) && !HAS_SNOOP(dev_priv)) {
		/* We cannot support coherent userptr objects on hw without
		 * LLC and broken snooping.
		 */
		return -ENODEV;
	}

	if (args->flags & ~(I915_USERPTR_READ_ONLY |
			    I915_USERPTR_UNSYNCHRONIZED))
		return -EINVAL;

	/*
	 * XXX: There is a prevalence of the assumption that we fit the
	 * object's page count inside a 32bit _signed_ variable. Let's document
	 * this and catch if we ever need to fix it. In the meantime, if you do
	 * spot such a local variable, please consider fixing!
	 *
	 * Aside from our own locals (for which we have no excuse!):
	 * - sg_table embeds unsigned int for num_pages
	 * - get_user_pages*() mixed ints with longs
	 */

	if (args->user_size >> PAGE_SHIFT > INT_MAX)
		return -E2BIG;

	if (overflows_type(args->user_size, obj->base.size))
		return -E2BIG;

	if (!args->user_size)
		return -EINVAL;

	if (offset_in_page(args->user_ptr | args->user_size))
		return -EINVAL;

	if (!access_ok((char __user *)(unsigned long)args->user_ptr, args->user_size))
		return -EFAULT;

	if (args->flags & I915_USERPTR_READ_ONLY) {
		/*
		 * On almost all of the older hw, we cannot tell the GPU that
		 * a page is readonly.
		 */
		if (!dev_priv->gt.vm->has_read_only)
			return -ENODEV;
	}

	obj = i915_gem_object_alloc();
	if (obj == NULL)
		return -ENOMEM;

	drm_gem_private_object_init(dev, &obj->base, args->user_size);
	i915_gem_object_init(obj, &i915_gem_userptr_ops, &lock_class);
	obj->read_domains = I915_GEM_DOMAIN_CPU;
	obj->write_domain = I915_GEM_DOMAIN_CPU;
	i915_gem_object_set_cache_coherency(obj, I915_CACHE_LLC);

	obj->userptr.ptr = args->user_ptr;
	if (args->flags & I915_USERPTR_READ_ONLY)
		i915_gem_object_set_readonly(obj);

	/* And keep a pointer to the current->mm for resolving the user pages
	 * at binding. This means that we need to hook into the mmu_notifier
	 * in order to detect if the mmu is destroyed.
	 */
	ret = i915_gem_userptr_init__mm_struct(obj);
	if (ret == 0)
		ret = i915_gem_userptr_init__mmu_notifier(obj, args->flags);
	if (ret == 0)
		ret = drm_gem_handle_create(file, &obj->base, &handle);

	/* drop reference from allocate - handle holds it now */
	i915_gem_object_put(obj);
	if (ret)
		return ret;

	args->handle = handle;
	return 0;
}