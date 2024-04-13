process_line(STREAM s, LINE_ORDER * os, uint32 present, RD_BOOL delta)
{
	if (present & 0x0001)
		in_uint16_le(s, os->mixmode);

	if (present & 0x0002)
		rdp_in_coord(s, &os->startx, delta);

	if (present & 0x0004)
		rdp_in_coord(s, &os->starty, delta);

	if (present & 0x0008)
		rdp_in_coord(s, &os->endx, delta);

	if (present & 0x0010)
		rdp_in_coord(s, &os->endy, delta);

	if (present & 0x0020)
		rdp_in_colour(s, &os->bgcolour);

	if (present & 0x0040)
		in_uint8(s, os->opcode);

	rdp_parse_pen(s, &os->pen, present >> 7);

	DEBUG(("LINE(op=0x%x,sx=%d,sy=%d,dx=%d,dy=%d,fg=0x%x)\n",
	       os->opcode, os->startx, os->starty, os->endx, os->endy, os->pen.colour));

	if (os->opcode < 0x01 || os->opcode > 0x10)
	{
		error("bad ROP2 0x%x\n", os->opcode);
		return;
	}

	ui_line(os->opcode - 1, os->startx, os->starty, os->endx, os->endy, &os->pen);
}