static int context_barrier_task(struct i915_gem_context *ctx,
				unsigned long engines,
				void (*task)(void *data),
				void *data)
{
	struct drm_i915_private *i915 = ctx->i915;
	struct context_barrier_task *cb;
	struct intel_context *ce;
	intel_wakeref_t wakeref;
	int err = 0;

	lockdep_assert_held(&i915->drm.struct_mutex);
	GEM_BUG_ON(!task);

	cb = kmalloc(sizeof(*cb), GFP_KERNEL);
	if (!cb)
		return -ENOMEM;

	i915_active_init(i915, &cb->base, cb_retire);
	i915_active_acquire(&cb->base);

	wakeref = intel_runtime_pm_get(i915);
	list_for_each_entry(ce, &ctx->active_engines, active_link) {
		struct intel_engine_cs *engine = ce->engine;
		struct i915_request *rq;

		if (!(ce->engine->mask & engines))
			continue;

		if (I915_SELFTEST_ONLY(context_barrier_inject_fault &
				       engine->mask)) {
			err = -ENXIO;
			break;
		}

		rq = i915_request_alloc(engine, ctx);
		if (IS_ERR(rq)) {
			err = PTR_ERR(rq);
			break;
		}

		err = i915_active_ref(&cb->base, rq->fence.context, rq);
		i915_request_add(rq);
		if (err)
			break;
	}
	intel_runtime_pm_put(i915, wakeref);

	cb->task = err ? NULL : task; /* caller needs to unwind instead */
	cb->data = data;

	i915_active_release(&cb->base);

	return err;
}