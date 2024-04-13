static inline unsigned long ad_mask(struct decode_cache *c)
{
	return (1UL << (c->ad_bytes << 3)) - 1;
}