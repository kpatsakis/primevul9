static int assign_hw_id(struct drm_i915_private *i915, unsigned int *out)
{
	int ret;

	lockdep_assert_held(&i915->contexts.mutex);

	/*
	 * We prefer to steal/stall ourselves and our users over that of the
	 * entire system. That may be a little unfair to our users, and
	 * even hurt high priority clients. The choice is whether to oomkill
	 * something else, or steal a context id.
	 */
	ret = new_hw_id(i915, GFP_KERNEL | __GFP_RETRY_MAYFAIL | __GFP_NOWARN);
	if (unlikely(ret < 0)) {
		ret = steal_hw_id(i915);
		if (ret < 0) /* once again for the correct errno code */
			ret = new_hw_id(i915, GFP_KERNEL);
		if (ret < 0)
			return ret;
	}

	*out = ret;
	return 0;
}