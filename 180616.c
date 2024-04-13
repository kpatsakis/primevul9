static inline void context_set_fault_enable(struct context_entry *context)
{
	context->lo &= (((u64)-1) << 2) | 1;
}