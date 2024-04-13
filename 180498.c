static inline void context_clear_entry(struct context_entry *context)
{
	context->lo = 0;
	context->hi = 0;
}