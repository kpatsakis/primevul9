static inline bool context_pasid_enabled(struct context_entry *context)
{
	return !!(context->lo & (1ULL << 11));
}