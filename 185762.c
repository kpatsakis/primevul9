last_request_on_engine(struct i915_timeline *timeline,
		       struct intel_engine_cs *engine)
{
	struct i915_request *rq;

	GEM_BUG_ON(timeline == &engine->timeline);

	rq = i915_active_request_raw(&timeline->last_request,
				     &engine->i915->drm.struct_mutex);
	if (rq && rq->engine == engine) {
		GEM_TRACE("last request for %s on engine %s: %llx:%llu\n",
			  timeline->name, engine->name,
			  rq->fence.context, rq->fence.seqno);
		GEM_BUG_ON(rq->timeline != timeline);
		return rq;
	}

	return NULL;
}