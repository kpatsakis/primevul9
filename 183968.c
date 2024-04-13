register_address(struct decode_cache *c, unsigned long base, unsigned long reg)
{
	return base + address_mask(c, reg);
}