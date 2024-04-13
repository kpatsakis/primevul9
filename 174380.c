rdp_out_brushcache_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BRUSHCACHE);
	out_uint16_le(s, RDP_CAPLEN_BRUSHCACHE);
	out_uint32_le(s, 1);	/* cache type */
}