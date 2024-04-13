process_memblt(STREAM s, MEMBLT_ORDER * os, uint32 present, RD_BOOL delta)
{
	RD_HBITMAP bitmap;

	if (present & 0x0001)
	{
		in_uint8(s, os->cache_id);
		in_uint8(s, os->colour_table);
	}

	if (present & 0x0002)
		rdp_in_coord(s, &os->x, delta);

	if (present & 0x0004)
		rdp_in_coord(s, &os->y, delta);

	if (present & 0x0008)
		rdp_in_coord(s, &os->cx, delta);

	if (present & 0x0010)
		rdp_in_coord(s, &os->cy, delta);

	if (present & 0x0020)
		in_uint8(s, os->opcode);

	if (present & 0x0040)
		rdp_in_coord(s, &os->srcx, delta);

	if (present & 0x0080)
		rdp_in_coord(s, &os->srcy, delta);

	if (present & 0x0100)
		in_uint16_le(s, os->cache_idx);

	DEBUG(("MEMBLT(op=0x%x,x=%d,y=%d,cx=%d,cy=%d,id=%d,idx=%d)\n",
	       os->opcode, os->x, os->y, os->cx, os->cy, os->cache_id, os->cache_idx));

	bitmap = cache_get_bitmap(os->cache_id, os->cache_idx);
	if (bitmap == NULL)
		return;

	ui_memblt(ROP2_S(os->opcode), os->x, os->y, os->cx, os->cy, bitmap, os->srcx, os->srcy);
}