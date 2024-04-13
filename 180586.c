static inline bool __context_present(struct context_entry *context)
{
	return (context->lo & 1);
}