static inline void context_set_copied(struct context_entry *context)
{
	context->hi |= (1ull << 3);
}