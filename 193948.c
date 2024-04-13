static inline struct io_context *create_io_context(gfp_t gfp_mask, int node)
{
	WARN_ON_ONCE(irqs_disabled());
	if (unlikely(!current->io_context))
		create_task_io_context(current, gfp_mask, node);
	return current->io_context;
}