int i915_gem_context_create_ioctl(struct drm_device *dev, void *data,
				  struct drm_file *file)
{
	struct drm_i915_private *i915 = to_i915(dev);
	struct drm_i915_gem_context_create *args = data;
	struct drm_i915_file_private *file_priv = file->driver_priv;
	struct i915_gem_context *ctx;
	int ret;

	if (!DRIVER_CAPS(i915)->has_logical_contexts)
		return -ENODEV;

	if (args->pad != 0)
		return -EINVAL;

	ret = i915_terminally_wedged(i915);
	if (ret)
		return ret;

	if (client_is_banned(file_priv)) {
		DRM_DEBUG("client %s[%d] banned from creating ctx\n",
			  current->comm,
			  pid_nr(get_task_pid(current, PIDTYPE_PID)));

		return -EIO;
	}

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	ctx = i915_gem_create_context(i915);
	mutex_unlock(&dev->struct_mutex);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = gem_context_register(ctx, file_priv);
	if (ret)
		goto err_ctx;

	args->ctx_id = ctx->user_handle;
	DRM_DEBUG("HW context %d created\n", args->ctx_id);

	return 0;

err_ctx:
	mutex_lock(&dev->struct_mutex);
	context_close(ctx);
	mutex_unlock(&dev->struct_mutex);
	return ret;
}