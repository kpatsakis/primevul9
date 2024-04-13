rdp_out_activate_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_ACTIVATE);
	out_uint16_le(s, RDP_CAPLEN_ACTIVATE);

	out_uint16(s, 0);	/* Help key */
	out_uint16(s, 0);	/* Help index key */
	out_uint16(s, 0);	/* Extended help key */
	out_uint16(s, 0);	/* Window activate */
}