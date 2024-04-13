rdp_parse_brush(STREAM s, BRUSH * brush, uint32 present)
{
	if (present & 1)
		in_uint8(s, brush->xorigin);

	if (present & 2)
		in_uint8(s, brush->yorigin);

	if (present & 4)
		in_uint8(s, brush->style);

	if (present & 8)
		in_uint8(s, brush->pattern[0]);

	if (present & 16)
		in_uint8a(s, &brush->pattern[1], 7);

	return s_check(s);
}