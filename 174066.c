has_ieee_hdr(struct ieee_types_generic *ie, u8 key)
{
	return (ie && ie->ieee_hdr.element_id == key);
}