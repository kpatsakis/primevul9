static void cb_retire(struct i915_active *base)
{
	struct context_barrier_task *cb = container_of(base, typeof(*cb), base);

	if (cb->task)
		cb->task(cb->data);

	i915_active_fini(&cb->base);
	kfree(cb);
}