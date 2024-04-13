static inline void context_set_translation_type(struct context_entry *context,
						unsigned long value)
{
	context->lo &= (((u64)-1) << 4) | 3;
	context->lo |= (value & 3) << 2;
}