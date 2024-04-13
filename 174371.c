process_new_pointer_pdu(STREAM s)
{
	int xor_bpp;

	in_uint16_le(s, xor_bpp);
	process_colour_pointer_common(s, xor_bpp);
}