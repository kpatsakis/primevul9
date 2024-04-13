process_rect(STREAM s, RECT_ORDER * os, uint32 present, RD_BOOL delta)
{
	uint32 i;
	if (present & 0x01)
		rdp_in_coord(s, &os->x, delta);

	if (present & 0x02)
		rdp_in_coord(s, &os->y, delta);

	if (present & 0x04)
		rdp_in_coord(s, &os->cx, delta);

	if (present & 0x08)
		rdp_in_coord(s, &os->cy, delta);

	if (present & 0x10)
	{
		in_uint8(s, i);
		os->colour = (os->colour & 0xffffff00) | i;
	}

	if (present & 0x20)
	{
		in_uint8(s, i);
		os->colour = (os->colour & 0xffff00ff) | (i << 8);
	}

	if (present & 0x40)
	{
		in_uint8(s, i);
		os->colour = (os->colour & 0xff00ffff) | (i << 16);
	}

	DEBUG(("RECT(x=%d,y=%d,cx=%d,cy=%d,fg=0x%x)\n", os->x, os->y, os->cx, os->cy, os->colour));

	ui_rect(os->x, os->y, os->cx, os->cy, os->colour);
}