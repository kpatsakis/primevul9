BOOL update_write_cache_bitmap_v3_order(wStream* s, CACHE_BITMAP_V3_ORDER* cache_bitmap_v3,
                                        UINT16* flags)
{
	BOOL rc;
	BYTE bitsPerPixelId;
	BITMAP_DATA_EX* bitmapData;

	if (!Stream_EnsureRemainingCapacity(
	        s, update_approximate_cache_bitmap_v3_order(cache_bitmap_v3, flags)))
		return FALSE;

	bitmapData = &cache_bitmap_v3->bitmapData;
	bitsPerPixelId = get_bpp_bmf(cache_bitmap_v3->bpp, &rc);
	if (!rc)
		return FALSE;
	*flags = (cache_bitmap_v3->cacheId & 0x00000003) |
	         ((cache_bitmap_v3->flags << 7) & 0x0000FF80) | ((bitsPerPixelId << 3) & 0x00000078);
	Stream_Write_UINT16(s, cache_bitmap_v3->cacheIndex); /* cacheIndex (2 bytes) */
	Stream_Write_UINT32(s, cache_bitmap_v3->key1);       /* key1 (4 bytes) */
	Stream_Write_UINT32(s, cache_bitmap_v3->key2);       /* key2 (4 bytes) */
	Stream_Write_UINT8(s, bitmapData->bpp);
	Stream_Write_UINT8(s, 0);                   /* reserved1 (1 byte) */
	Stream_Write_UINT8(s, 0);                   /* reserved2 (1 byte) */
	Stream_Write_UINT8(s, bitmapData->codecID); /* codecID (1 byte) */
	Stream_Write_UINT16(s, bitmapData->width);  /* width (2 bytes) */
	Stream_Write_UINT16(s, bitmapData->height); /* height (2 bytes) */
	Stream_Write_UINT32(s, bitmapData->length); /* length (4 bytes) */
	Stream_Write(s, bitmapData->data, bitmapData->length);
	return TRUE;
}