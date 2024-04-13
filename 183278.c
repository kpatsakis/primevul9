static inline void context_set_sm_pre(struct context_entry *context)
{
	context->lo |= (1 << 4);
}