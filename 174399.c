process_bmpcache2(STREAM s, uint16 flags, RD_BOOL compressed)
{
	RD_HBITMAP bitmap;
	int y;
	uint8 cache_id, cache_idx_low, width, height, Bpp;
	uint16 cache_idx, bufsize;
	uint8 *data, *bmpdata, *bitmap_id;

	bitmap_id = NULL;	/* prevent compiler warning */
	cache_id = flags & ID_MASK;
	Bpp = ((flags & MODE_MASK) >> MODE_SHIFT) - 2;

	if (flags & PERSIST)
	{
		in_uint8p(s, bitmap_id, 8);
	}

	if (flags & SQUARE)
	{
		in_uint8(s, width);
		height = width;
	}
	else
	{
		in_uint8(s, width);
		in_uint8(s, height);
	}

	in_uint16_be(s, bufsize);
	bufsize &= BUFSIZE_MASK;
	in_uint8(s, cache_idx);

	if (cache_idx & LONG_FORMAT)
	{
		in_uint8(s, cache_idx_low);
		cache_idx = ((cache_idx ^ LONG_FORMAT) << 8) + cache_idx_low;
	}

	in_uint8p(s, data, bufsize);

	DEBUG(("BMPCACHE2(compr=%d,flags=%x,cx=%d,cy=%d,id=%d,idx=%d,Bpp=%d,bs=%d)\n",
	       compressed, flags, width, height, cache_id, cache_idx, Bpp, bufsize));

	bmpdata = (uint8 *) xmalloc(width * height * Bpp);

	if (compressed)
	{
		if (!bitmap_decompress(bmpdata, width, height, data, bufsize, Bpp))
		{
			DEBUG(("Failed to decompress bitmap data\n"));
			xfree(bmpdata);
			return;
		}
	}
	else
	{
		for (y = 0; y < height; y++)
			memcpy(&bmpdata[(height - y - 1) * (width * Bpp)],
			       &data[y * (width * Bpp)], width * Bpp);
	}

	bitmap = ui_create_bitmap(width, height, bmpdata);

	if (bitmap)
	{
		cache_put_bitmap(cache_id, cache_idx, bitmap);
		if (flags & PERSIST)
			pstcache_save_bitmap(cache_id, cache_idx, bitmap_id, width, height,
					     width * height * Bpp, bmpdata);
	}
	else
	{
		DEBUG(("process_bmpcache2: ui_create_bitmap failed\n"));
	}

	xfree(bmpdata);
}