static inline int new_hw_id(struct drm_i915_private *i915, gfp_t gfp)
{
	unsigned int max;

	lockdep_assert_held(&i915->contexts.mutex);

	if (INTEL_GEN(i915) >= 11)
		max = GEN11_MAX_CONTEXT_HW_ID;
	else if (USES_GUC_SUBMISSION(i915))
		/*
		 * When using GuC in proxy submission, GuC consumes the
		 * highest bit in the context id to indicate proxy submission.
		 */
		max = MAX_GUC_CONTEXT_HW_ID;
	else
		max = MAX_CONTEXT_HW_ID;

	return ida_simple_get(&i915->contexts.hw_ida, 0, max, gfp);
}