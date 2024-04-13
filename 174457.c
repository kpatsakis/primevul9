process_destblt(STREAM s, DESTBLT_ORDER * os, uint32 present, RD_BOOL delta)
{
	if (present & 0x01)
		rdp_in_coord(s, &os->x, delta);

	if (present & 0x02)
		rdp_in_coord(s, &os->y, delta);

	if (present & 0x04)
		rdp_in_coord(s, &os->cx, delta);

	if (present & 0x08)
		rdp_in_coord(s, &os->cy, delta);

	if (present & 0x10)
		in_uint8(s, os->opcode);

	DEBUG(("DESTBLT(op=0x%x,x=%d,y=%d,cx=%d,cy=%d)\n",
	       os->opcode, os->x, os->y, os->cx, os->cy));

	ui_destblt(ROP2_S(os->opcode), os->x, os->y, os->cx, os->cy);
}