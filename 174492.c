process_ellipse2(STREAM s, ELLIPSE2_ORDER * os, uint32 present, RD_BOOL delta)
{
	BRUSH brush;

	if (present & 0x0001)
		rdp_in_coord(s, &os->left, delta);

	if (present & 0x0002)
		rdp_in_coord(s, &os->top, delta);

	if (present & 0x0004)
		rdp_in_coord(s, &os->right, delta);

	if (present & 0x0008)
		rdp_in_coord(s, &os->bottom, delta);

	if (present & 0x0010)
		in_uint8(s, os->opcode);

	if (present & 0x0020)
		in_uint8(s, os->fillmode);

	if (present & 0x0040)
		rdp_in_colour(s, &os->bgcolour);

	if (present & 0x0080)
		rdp_in_colour(s, &os->fgcolour);

	rdp_parse_brush(s, &os->brush, present >> 8);

	DEBUG(("ELLIPSE2(l=%d,t=%d,r=%d,b=%d,op=0x%x,fm=%d,bs=%d,bg=0x%x,fg=0x%x)\n",
	       os->left, os->top, os->right, os->bottom, os->opcode, os->fillmode, os->brush.style,
	       os->bgcolour, os->fgcolour));

	setup_brush(&brush, &os->brush);

	ui_ellipse(os->opcode - 1, os->fillmode, os->left, os->top, os->right - os->left,
		   os->bottom - os->top, &brush, os->bgcolour, os->fgcolour);
}