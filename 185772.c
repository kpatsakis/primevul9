static int set_sseu(struct i915_gem_context *ctx,
		    struct drm_i915_gem_context_param *args)
{
	struct drm_i915_private *i915 = ctx->i915;
	struct drm_i915_gem_context_param_sseu user_sseu;
	struct intel_engine_cs *engine;
	struct intel_sseu sseu;
	int ret;

	if (args->size < sizeof(user_sseu))
		return -EINVAL;

	if (!IS_GEN(i915, 11))
		return -ENODEV;

	if (copy_from_user(&user_sseu, u64_to_user_ptr(args->value),
			   sizeof(user_sseu)))
		return -EFAULT;

	if (user_sseu.flags || user_sseu.rsvd)
		return -EINVAL;

	engine = intel_engine_lookup_user(i915,
					  user_sseu.engine_class,
					  user_sseu.engine_instance);
	if (!engine)
		return -EINVAL;

	/* Only render engine supports RPCS configuration. */
	if (engine->class != RENDER_CLASS)
		return -ENODEV;

	ret = user_to_context_sseu(i915, &user_sseu, &sseu);
	if (ret)
		return ret;

	ret = i915_gem_context_reconfigure_sseu(ctx, engine, sseu);
	if (ret)
		return ret;

	args->size = sizeof(user_sseu);

	return 0;
}