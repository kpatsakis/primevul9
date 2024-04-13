static CACHE_BITMAP_V3_ORDER* update_read_cache_bitmap_v3_order(rdpUpdate* update, wStream* s,
                                                                UINT16 flags)
{
	BOOL rc;
	BYTE bitsPerPixelId;
	BITMAP_DATA_EX* bitmapData;
	UINT32 new_len;
	BYTE* new_data;
	CACHE_BITMAP_V3_ORDER* cache_bitmap_v3;

	if (!update || !s)
		return NULL;

	cache_bitmap_v3 = calloc(1, sizeof(CACHE_BITMAP_V3_ORDER));

	if (!cache_bitmap_v3)
		goto fail;

	cache_bitmap_v3->cacheId = flags & 0x00000003;
	cache_bitmap_v3->flags = (flags & 0x0000FF80) >> 7;
	bitsPerPixelId = (flags & 0x00000078) >> 3;
	cache_bitmap_v3->bpp = get_cbr2_bpp(bitsPerPixelId, &rc);
	if (!rc)
		goto fail;

	if (Stream_GetRemainingLength(s) < 21)
		goto fail;

	Stream_Read_UINT16(s, cache_bitmap_v3->cacheIndex); /* cacheIndex (2 bytes) */
	Stream_Read_UINT32(s, cache_bitmap_v3->key1);       /* key1 (4 bytes) */
	Stream_Read_UINT32(s, cache_bitmap_v3->key2);       /* key2 (4 bytes) */
	bitmapData = &cache_bitmap_v3->bitmapData;
	Stream_Read_UINT8(s, bitmapData->bpp);

	if ((bitmapData->bpp < 1) || (bitmapData->bpp > 32))
	{
		WLog_Print(update->log, WLOG_ERROR, "invalid bpp value %" PRIu32 "", bitmapData->bpp);
		goto fail;
	}

	Stream_Seek_UINT8(s);                      /* reserved1 (1 byte) */
	Stream_Seek_UINT8(s);                      /* reserved2 (1 byte) */
	Stream_Read_UINT8(s, bitmapData->codecID); /* codecID (1 byte) */
	Stream_Read_UINT16(s, bitmapData->width);  /* width (2 bytes) */
	Stream_Read_UINT16(s, bitmapData->height); /* height (2 bytes) */
	Stream_Read_UINT32(s, new_len);            /* length (4 bytes) */

	if ((new_len == 0) || (Stream_GetRemainingLength(s) < new_len))
		goto fail;

	new_data = (BYTE*)realloc(bitmapData->data, new_len);

	if (!new_data)
		goto fail;

	bitmapData->data = new_data;
	bitmapData->length = new_len;
	Stream_Read(s, bitmapData->data, bitmapData->length);
	return cache_bitmap_v3;
fail:
	free_cache_bitmap_v3_order(update->context, cache_bitmap_v3);
	return NULL;
}