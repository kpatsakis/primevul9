static inline void context_clear_pasid_enable(struct context_entry *context)
{
	context->lo &= ~(1ULL << 11);
}