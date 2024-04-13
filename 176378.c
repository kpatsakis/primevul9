void i915_gem_cleanup_userptr(struct drm_i915_private *dev_priv)
{
	destroy_workqueue(dev_priv->mm.userptr_wq);
}