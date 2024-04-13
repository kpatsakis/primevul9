int i915_gem_switch_to_kernel_context(struct drm_i915_private *i915,
				      unsigned long mask)
{
	struct intel_engine_cs *engine;

	GEM_TRACE("awake?=%s\n", yesno(i915->gt.awake));

	lockdep_assert_held(&i915->drm.struct_mutex);
	GEM_BUG_ON(!i915->kernel_context);

	/* Inoperable, so presume the GPU is safely pointing into the void! */
	if (i915_terminally_wedged(i915))
		return 0;

	for_each_engine_masked(engine, i915, mask, mask) {
		struct intel_ring *ring;
		struct i915_request *rq;

		rq = i915_request_alloc(engine, i915->kernel_context);
		if (IS_ERR(rq))
			return PTR_ERR(rq);

		/* Queue this switch after all other activity */
		list_for_each_entry(ring, &i915->gt.active_rings, active_link) {
			struct i915_request *prev;

			prev = last_request_on_engine(ring->timeline, engine);
			if (!prev)
				continue;

			if (prev->gem_context == i915->kernel_context)
				continue;

			GEM_TRACE("add barrier on %s for %llx:%lld\n",
				  engine->name,
				  prev->fence.context,
				  prev->fence.seqno);
			i915_sw_fence_await_sw_fence_gfp(&rq->submit,
							 &prev->submit,
							 I915_FENCE_GFP);
		}

		i915_request_add(rq);
	}

	return 0;
}