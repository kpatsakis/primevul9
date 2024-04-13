rdp_in_colour(STREAM s, uint32 * colour)
{
	uint32 i;
	in_uint8(s, i);
	*colour = i;
	in_uint8(s, i);
	*colour |= i << 8;
	in_uint8(s, i);
	*colour |= i << 16;
}