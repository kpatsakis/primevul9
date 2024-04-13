static inline void context_set_present(struct context_entry *context)
{
	context->lo |= 1;
}