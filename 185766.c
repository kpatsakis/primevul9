static bool needs_preempt_context(struct drm_i915_private *i915)
{
	return HAS_LOGICAL_RING_PREEMPTION(i915);
}