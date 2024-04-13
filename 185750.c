int i915_gem_context_getparam_ioctl(struct drm_device *dev, void *data,
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
		args->size = 0;
		args->value = test_bit(UCONTEXT_NO_ZEROMAP, &ctx->user_flags);
		break;
	case I915_CONTEXT_PARAM_GTT_SIZE:
		args->size = 0;

		if (ctx->ppgtt)
			args->value = ctx->ppgtt->vm.total;
		else if (to_i915(dev)->mm.aliasing_ppgtt)
			args->value = to_i915(dev)->mm.aliasing_ppgtt->vm.total;
		else
			args->value = to_i915(dev)->ggtt.vm.total;
		break;
	case I915_CONTEXT_PARAM_NO_ERROR_CAPTURE:
		args->size = 0;
		args->value = i915_gem_context_no_error_capture(ctx);
		break;
	case I915_CONTEXT_PARAM_BANNABLE:
		args->size = 0;
		args->value = i915_gem_context_is_bannable(ctx);
		break;
	case I915_CONTEXT_PARAM_RECOVERABLE:
		args->size = 0;
		args->value = i915_gem_context_is_recoverable(ctx);
		break;
	case I915_CONTEXT_PARAM_PRIORITY:
		args->size = 0;
		args->value = ctx->sched.priority >> I915_USER_PRIORITY_SHIFT;
		break;
	case I915_CONTEXT_PARAM_SSEU:
		ret = get_sseu(ctx, args);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	i915_gem_context_put(ctx);
	return ret;
}