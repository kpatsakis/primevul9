rdp_out_bitmap_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_BITMAP);
	out_uint16_le(s, RDP_CAPLEN_BITMAP);

	out_uint16_le(s, g_server_depth);	/* Preferred colour depth */
	out_uint16_le(s, 1);	/* Receive 1 BPP */
	out_uint16_le(s, 1);	/* Receive 4 BPP */
	out_uint16_le(s, 1);	/* Receive 8 BPP */
	out_uint16_le(s, 800);	/* Desktop width */
	out_uint16_le(s, 600);	/* Desktop height */
	out_uint16(s, 0);	/* Pad */
	out_uint16(s, 1);	/* Allow resize */
	out_uint16_le(s, g_bitmap_compression ? 1 : 0);	/* Support compression */
	out_uint16(s, 0);	/* Unknown */
	out_uint16_le(s, 1);	/* Unknown */
	out_uint16(s, 0);	/* Pad */
}