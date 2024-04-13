has_vendor_hdr(struct ieee_types_vendor_specific *ie, u8 key)
{
	return (ie && ie->vend_hdr.element_id == key);
}