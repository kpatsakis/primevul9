__i915_gem_context_reconfigure_sseu(struct i915_gem_context *ctx,
				    struct intel_engine_cs *engine,
				    struct intel_sseu sseu)
{
	struct intel_context *ce;
	int ret = 0;

	GEM_BUG_ON(INTEL_GEN(ctx->i915) < 8);
	GEM_BUG_ON(engine->id != RCS0);

	ce = intel_context_pin_lock(ctx, engine);
	if (IS_ERR(ce))
		return PTR_ERR(ce);

	/* Nothing to do if unmodified. */
	if (!memcmp(&ce->sseu, &sseu, sizeof(sseu)))
		goto unlock;

	ret = gen8_modify_rpcs(ce, sseu);
	if (!ret)
		ce->sseu = sseu;

unlock:
	intel_context_pin_unlock(ce);
	return ret;
}