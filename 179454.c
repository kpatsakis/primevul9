gen8_dispatch_bsd_engine(struct drm_i915_private *dev_priv,
			 struct drm_file *file)
{
	struct drm_i915_file_private *file_priv = file->driver_priv;

	/* Check whether the file_priv has already selected one ring. */
	if ((int)file_priv->bsd_engine < 0)
		file_priv->bsd_engine = atomic_fetch_xor(1,
			 &dev_priv->mm.bsd_engine_dispatch_index);

	return file_priv->bsd_engine;
}