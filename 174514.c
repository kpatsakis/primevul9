process_ellipse(STREAM s, ELLIPSE_ORDER * os, uint32 present, RD_BOOL delta)
{
	if (present & 0x01)
		rdp_in_coord(s, &os->left, delta);

	if (present & 0x02)
		rdp_in_coord(s, &os->top, delta);

	if (present & 0x04)
		rdp_in_coord(s, &os->right, delta);

	if (present & 0x08)
		rdp_in_coord(s, &os->bottom, delta);

	if (present & 0x10)
		in_uint8(s, os->opcode);

	if (present & 0x20)
		in_uint8(s, os->fillmode);

	if (present & 0x40)
		rdp_in_colour(s, &os->fgcolour);

	DEBUG(("ELLIPSE(l=%d,t=%d,r=%d,b=%d,op=0x%x,fm=%d,fg=0x%x)\n", os->left, os->top,
	       os->right, os->bottom, os->opcode, os->fillmode, os->fgcolour));

	ui_ellipse(os->opcode - 1, os->fillmode, os->left, os->top, os->right - os->left,
		   os->bottom - os->top, NULL, 0, os->fgcolour);
}