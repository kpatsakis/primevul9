static int get_sseu(struct i915_gem_context *ctx,
		    struct drm_i915_gem_context_param *args)
{
	struct drm_i915_gem_context_param_sseu user_sseu;
	struct intel_engine_cs *engine;
	struct intel_context *ce;

	if (args->size == 0)
		goto out;
	else if (args->size < sizeof(user_sseu))
		return -EINVAL;

	if (copy_from_user(&user_sseu, u64_to_user_ptr(args->value),
			   sizeof(user_sseu)))
		return -EFAULT;

	if (user_sseu.flags || user_sseu.rsvd)
		return -EINVAL;

	engine = intel_engine_lookup_user(ctx->i915,
					  user_sseu.engine_class,
					  user_sseu.engine_instance);
	if (!engine)
		return -EINVAL;

	ce = intel_context_pin_lock(ctx, engine); /* serialises with set_sseu */
	if (IS_ERR(ce))
		return PTR_ERR(ce);

	user_sseu.slice_mask = ce->sseu.slice_mask;
	user_sseu.subslice_mask = ce->sseu.subslice_mask;
	user_sseu.min_eus_per_subslice = ce->sseu.min_eus_per_subslice;
	user_sseu.max_eus_per_subslice = ce->sseu.max_eus_per_subslice;

	intel_context_pin_unlock(ce);

	if (copy_to_user(u64_to_user_ptr(args->value), &user_sseu,
			 sizeof(user_sseu)))
		return -EFAULT;

out:
	args->size = sizeof(user_sseu);

	return 0;
}