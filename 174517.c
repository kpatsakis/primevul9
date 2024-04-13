rdp_out_share_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_SHARE);
	out_uint16_le(s, RDP_CAPLEN_SHARE);

	out_uint16(s, 0);	/* userid */
	out_uint16(s, 0);	/* pad */
}