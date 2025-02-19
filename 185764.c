void i915_gem_context_close(struct drm_file *file)
{
	struct drm_i915_file_private *file_priv = file->driver_priv;

	lockdep_assert_held(&file_priv->dev_priv->drm.struct_mutex);

	idr_for_each(&file_priv->context_idr, context_idr_cleanup, NULL);
	idr_destroy(&file_priv->context_idr);
	mutex_destroy(&file_priv->context_idr_lock);
}