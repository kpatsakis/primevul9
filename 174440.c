rdp_out_pointer_caps(STREAM s)
{
	out_uint16_le(s, RDP_CAPSET_POINTER);
	out_uint16_le(s, RDP_CAPLEN_POINTER);

	out_uint16(s, 0);	/* Colour pointer */
	out_uint16_le(s, 20);	/* Cache size */
}