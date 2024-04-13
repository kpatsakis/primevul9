static int eb_select_context(struct i915_execbuffer *eb)
{
	struct i915_gem_context *ctx;

	ctx = i915_gem_context_lookup(eb->file->driver_priv, eb->args->rsvd1);
	if (unlikely(!ctx))
		return -ENOENT;

	eb->ctx = ctx;
	if (ctx->ppgtt) {
		eb->vm = &ctx->ppgtt->vm;
		eb->invalid_flags |= EXEC_OBJECT_NEEDS_GTT;
	} else {
		eb->vm = &eb->i915->ggtt.vm;
	}

	eb->context_flags = 0;
	if (test_bit(UCONTEXT_NO_ZEROMAP, &ctx->user_flags))
		eb->context_flags |= __EXEC_OBJECT_NEEDS_BIAS;

	return 0;
}