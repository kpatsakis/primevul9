rdp_out_colcache_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_COLCACHE);
	out_uint16_le(s, RDP_CAPLEN_COLCACHE);

	out_uint16_le(s, 6);	/* cache size */
	out_uint16(s, 0);	/* pad */
}