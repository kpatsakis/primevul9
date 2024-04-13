static inline bool context_copied(struct context_entry *context)
{
	return !!(context->hi & (1ULL << 3));
}