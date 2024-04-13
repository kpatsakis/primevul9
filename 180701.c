static inline void context_set_address_width(struct context_entry *context,
					     unsigned long value)
{
	context->hi |= value & 7;
}