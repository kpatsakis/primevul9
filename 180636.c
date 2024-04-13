static inline int context_domain_id(struct context_entry *c)
{
	return((c->hi >> 8) & 0xffff);
}