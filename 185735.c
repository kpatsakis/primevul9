static void context_close(struct i915_gem_context *ctx)
{
	i915_gem_context_set_closed(ctx);

	/*
	 * This context will never again be assinged to HW, so we can
	 * reuse its ID for the next context.
	 */
	release_hw_id(ctx);

	/*
	 * The LUT uses the VMA as a backpointer to unref the object,
	 * so we need to clear the LUT before we close all the VMA (inside
	 * the ppgtt).
	 */
	lut_close(ctx);
	if (ctx->ppgtt)
		i915_ppgtt_close(&ctx->ppgtt->vm);

	ctx->file_priv = ERR_PTR(-EBADF);
	i915_gem_context_put(ctx);
}