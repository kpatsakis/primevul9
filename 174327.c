rdp_out_bmpcache_caps(STREAM s)
{
	int Bpp;
	out_uint16_le(s, RDP_CAPSET_BMPCACHE);
	out_uint16_le(s, RDP_CAPLEN_BMPCACHE);

	Bpp = (g_server_depth + 7) / 8;	/* bytes per pixel */
	out_uint8s(s, 24);	/* unused */
	out_uint16_le(s, 0x258);	/* entries */
	out_uint16_le(s, 0x100 * Bpp);	/* max cell size */
	out_uint16_le(s, 0x12c);	/* entries */
	out_uint16_le(s, 0x400 * Bpp);	/* max cell size */
	out_uint16_le(s, 0x106);	/* entries */
	out_uint16_le(s, 0x1000 * Bpp);	/* max cell size */
}