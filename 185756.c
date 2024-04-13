gen8_modify_rpcs(struct intel_context *ce, struct intel_sseu sseu)
{
	struct drm_i915_private *i915 = ce->engine->i915;
	struct i915_request *rq, *prev;
	intel_wakeref_t wakeref;
	int ret;

	lockdep_assert_held(&ce->pin_mutex);

	/*
	 * If the context is not idle, we have to submit an ordered request to
	 * modify its context image via the kernel context (writing to our own
	 * image, or into the registers directory, does not stick). Pristine
	 * and idle contexts will be configured on pinning.
	 */
	if (!intel_context_is_pinned(ce))
		return 0;

	/* Submitting requests etc needs the hw awake. */
	wakeref = intel_runtime_pm_get(i915);

	rq = i915_request_alloc(ce->engine, i915->kernel_context);
	if (IS_ERR(rq)) {
		ret = PTR_ERR(rq);
		goto out_put;
	}

	/* Queue this switch after all other activity by this context. */
	prev = i915_active_request_raw(&ce->ring->timeline->last_request,
				       &i915->drm.struct_mutex);
	if (prev && !i915_request_completed(prev)) {
		ret = i915_request_await_dma_fence(rq, &prev->fence);
		if (ret < 0)
			goto out_add;
	}

	/* Order all following requests to be after. */
	ret = i915_timeline_set_barrier(ce->ring->timeline, rq);
	if (ret)
		goto out_add;

	ret = gen8_emit_rpcs_config(rq, ce, sseu);
	if (ret)
		goto out_add;

	/*
	 * Guarantee context image and the timeline remains pinned until the
	 * modifying request is retired by setting the ce activity tracker.
	 *
	 * But we only need to take one pin on the account of it. Or in other
	 * words transfer the pinned ce object to tracked active request.
	 */
	if (!i915_active_request_isset(&ce->active_tracker))
		__intel_context_pin(ce);
	__i915_active_request_set(&ce->active_tracker, rq);

out_add:
	i915_request_add(rq);
out_put:
	intel_runtime_pm_put(i915, wakeref);

	return ret;
}