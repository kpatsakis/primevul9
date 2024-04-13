rdp_in_coord(STREAM s, sint16 * coord, RD_BOOL delta)
{
	sint8 change;

	if (delta)
	{
		in_uint8(s, change);
		*coord += change;
	}
	else
	{
		in_uint16_le(s, *coord);
	}
}