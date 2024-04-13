int i915_gem_context_setparam_ioctl(struct drm_device *dev, void *data,
				    struct drm_file *file)
{
	struct drm_i915_file_private *file_priv = file->driver_priv;
	struct drm_i915_gem_context_param *args = data;
	struct i915_gem_context *ctx;
	int ret = 0;

	ctx = i915_gem_context_lookup(file_priv, args->ctx_id);
	if (!ctx)
		return -ENOENT;

	switch (args->param) {
	case I915_CONTEXT_PARAM_BAN_PERIOD:
		ret = -EINVAL;
		break;
	case I915_CONTEXT_PARAM_NO_ZEROMAP:
		if (args->size)
			ret = -EINVAL;
		else if (args->value)
			set_bit(UCONTEXT_NO_ZEROMAP, &ctx->user_flags);
		else
			clear_bit(UCONTEXT_NO_ZEROMAP, &ctx->user_flags);
		break;
	case I915_CONTEXT_PARAM_NO_ERROR_CAPTURE:
		if (args->size)
			ret = -EINVAL;
		else if (args->value)
			i915_gem_context_set_no_error_capture(ctx);
		else
			i915_gem_context_clear_no_error_capture(ctx);
		break;
	case I915_CONTEXT_PARAM_BANNABLE:
		if (args->size)
			ret = -EINVAL;
		else if (!capable(CAP_SYS_ADMIN) && !args->value)
			ret = -EPERM;
		else if (args->value)
			i915_gem_context_set_bannable(ctx);
		else
			i915_gem_context_clear_bannable(ctx);
		break;

	case I915_CONTEXT_PARAM_RECOVERABLE:
		if (args->size)
			ret = -EINVAL;
		else if (args->value)
			i915_gem_context_set_recoverable(ctx);
		else
			i915_gem_context_clear_recoverable(ctx);
		break;

	case I915_CONTEXT_PARAM_PRIORITY:
		{
			s64 priority = args->value;

			if (args->size)
				ret = -EINVAL;
			else if (!(to_i915(dev)->caps.scheduler & I915_SCHEDULER_CAP_PRIORITY))
				ret = -ENODEV;
			else if (priority > I915_CONTEXT_MAX_USER_PRIORITY ||
				 priority < I915_CONTEXT_MIN_USER_PRIORITY)
				ret = -EINVAL;
			else if (priority > I915_CONTEXT_DEFAULT_PRIORITY &&
				 !capable(CAP_SYS_NICE))
				ret = -EPERM;
			else
				ctx->sched.priority =
					I915_USER_PRIORITY(priority);
		}
		break;
	case I915_CONTEXT_PARAM_SSEU:
		ret = set_sseu(ctx, args);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	i915_gem_context_put(ctx);
	return ret;
}