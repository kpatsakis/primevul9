address_mask(struct decode_cache *c, unsigned long reg)
{
	if (c->ad_bytes == sizeof(unsigned long))
		return reg;
	else
		return reg & ad_mask(c);
}