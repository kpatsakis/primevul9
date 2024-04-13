process_raw_bmpcache(STREAM s)
{
	RD_HBITMAP bitmap;
	uint16 cache_idx, bufsize;
	uint8 cache_id, width, height, bpp, Bpp;
	uint8 *data, *inverted;
	int y;

	in_uint8(s, cache_id);
	in_uint8s(s, 1);	/* pad */
	in_uint8(s, width);
	in_uint8(s, height);
	in_uint8(s, bpp);
	Bpp = (bpp + 7) / 8;
	in_uint16_le(s, bufsize);
	in_uint16_le(s, cache_idx);
	in_uint8p(s, data, bufsize);

	DEBUG(("RAW_BMPCACHE(cx=%d,cy=%d,id=%d,idx=%d)\n", width, height, cache_id, cache_idx));
	inverted = (uint8 *) xmalloc(width * height * Bpp);
	for (y = 0; y < height; y++)
	{
		memcpy(&inverted[(height - y - 1) * (width * Bpp)], &data[y * (width * Bpp)],
		       width * Bpp);
	}

	bitmap = ui_create_bitmap(width, height, inverted);
	xfree(inverted);
	cache_put_bitmap(cache_id, cache_idx, bitmap);
}