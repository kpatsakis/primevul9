static inline void context_set_sm_dte(struct context_entry *context)
{
	context->lo |= (1 << 2);
}