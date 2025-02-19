static int gem_context_register(struct i915_gem_context *ctx,
				struct drm_i915_file_private *fpriv)
{
	int ret;

	ctx->file_priv = fpriv;
	if (ctx->ppgtt)
		ctx->ppgtt->vm.file = fpriv;

	ctx->pid = get_task_pid(current, PIDTYPE_PID);
	ctx->name = kasprintf(GFP_KERNEL, "%s[%d]",
			      current->comm, pid_nr(ctx->pid));
	if (!ctx->name) {
		ret = -ENOMEM;
		goto err_pid;
	}

	/* And finally expose ourselves to userspace via the idr */
	mutex_lock(&fpriv->context_idr_lock);
	ret = idr_alloc(&fpriv->context_idr, ctx,
			DEFAULT_CONTEXT_HANDLE, 0, GFP_KERNEL);
	if (ret >= 0)
		ctx->user_handle = ret;
	mutex_unlock(&fpriv->context_idr_lock);
	if (ret < 0)
		goto err_name;

	return 0;

err_name:
	kfree(fetch_and_zero(&ctx->name));
err_pid:
	put_pid(fetch_and_zero(&ctx->pid));
	return ret;
}