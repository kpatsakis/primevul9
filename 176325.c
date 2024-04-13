int i915_gem_init_userptr(struct drm_i915_private *dev_priv)
{
	mutex_init(&dev_priv->mm_lock);
	hash_init(dev_priv->mm_structs);

	dev_priv->mm.userptr_wq =
		alloc_workqueue("i915-userptr-acquire",
				WQ_HIGHPRI | WQ_UNBOUND,
				0);
	if (!dev_priv->mm.userptr_wq)
		return -ENOMEM;

	return 0;
}