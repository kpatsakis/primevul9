static inline void jmp_rel(struct decode_cache *c, int rel)
{
	register_address_increment(c, &c->eip, rel);
}