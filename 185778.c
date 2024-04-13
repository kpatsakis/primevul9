static void contexts_free_worker(struct work_struct *work)
{
	struct drm_i915_private *i915 =
		container_of(work, typeof(*i915), contexts.free_work);

	mutex_lock(&i915->drm.struct_mutex);
	contexts_free(i915);
	mutex_unlock(&i915->drm.struct_mutex);
}