process_polygon(STREAM s, POLYGON_ORDER * os, uint32 present, RD_BOOL delta)
{
	int index, data, next;
	uint8 flags = 0;
	RD_POINT *points;

	if (present & 0x01)
		rdp_in_coord(s, &os->x, delta);

	if (present & 0x02)
		rdp_in_coord(s, &os->y, delta);

	if (present & 0x04)
		in_uint8(s, os->opcode);

	if (present & 0x08)
		in_uint8(s, os->fillmode);

	if (present & 0x10)
		rdp_in_colour(s, &os->fgcolour);

	if (present & 0x20)
		in_uint8(s, os->npoints);

	if (present & 0x40)
	{
		in_uint8(s, os->datasize);
		in_uint8a(s, os->data, os->datasize);
	}

	DEBUG(("POLYGON(x=%d,y=%d,op=0x%x,fm=%d,fg=0x%x,n=%d,sz=%d)\n",
	       os->x, os->y, os->opcode, os->fillmode, os->fgcolour, os->npoints, os->datasize));

	DEBUG(("Data: "));

	for (index = 0; index < os->datasize; index++)
		DEBUG(("%02x ", os->data[index]));

	DEBUG(("\n"));

	if (os->opcode < 0x01 || os->opcode > 0x10)
	{
		error("bad ROP2 0x%x\n", os->opcode);
		return;
	}

	points = (RD_POINT *) xmalloc((os->npoints + 1) * sizeof(RD_POINT));
	memset(points, 0, (os->npoints + 1) * sizeof(RD_POINT));

	points[0].x = os->x;
	points[0].y = os->y;

	index = 0;
	data = ((os->npoints - 1) / 4) + 1;
	for (next = 1; (next <= os->npoints) && (next < 256) && (data < os->datasize); next++)
	{
		if ((next - 1) % 4 == 0)
			flags = os->data[index++];

		if (~flags & 0x80)
			points[next].x = parse_delta(os->data, &data);

		if (~flags & 0x40)
			points[next].y = parse_delta(os->data, &data);

		flags <<= 2;
	}

	if (next - 1 == os->npoints)
		ui_polygon(os->opcode - 1, os->fillmode, points, os->npoints + 1, NULL, 0,
			   os->fgcolour);
	else
		error("polygon parse error\n");

	xfree(points);
}