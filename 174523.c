process_colour_pointer_common(STREAM s, int bpp)
{
	uint16 width, height, cache_idx, masklen, datalen;
	uint16 x, y;
	uint8 *mask;
	uint8 *data;
	RD_HCURSOR cursor;

	in_uint16_le(s, cache_idx);
	in_uint16_le(s, x);
	in_uint16_le(s, y);
	in_uint16_le(s, width);
	in_uint16_le(s, height);
	in_uint16_le(s, masklen);
	in_uint16_le(s, datalen);
	in_uint8p(s, data, datalen);
	in_uint8p(s, mask, masklen);
	if ((width != 32) || (height != 32))
	{
		warning("process_colour_pointer_common: " "width %d height %d\n", width, height);
	}

	/* keep hotspot within cursor bounding box */
	x = MIN(x, width - 1);
	y = MIN(y, height - 1);
	cursor = ui_create_cursor(x, y, width, height, mask, data, bpp);
	ui_set_cursor(cursor);
	cache_put_cursor(cache_idx, cursor);
}