static BOOL rfx_write_message_region(RFX_CONTEXT* context, wStream* s, RFX_MESSAGE* message)
{
	int i;
	UINT32 blockLen;
	blockLen = 15 + (message->numRects * 8);

	if (!Stream_EnsureRemainingCapacity(s, blockLen))
		return FALSE;

	Stream_Write_UINT16(s, WBT_REGION);        /* CodecChannelT.blockType (2 bytes) */
	Stream_Write_UINT32(s, blockLen);          /* set CodecChannelT.blockLen (4 bytes) */
	Stream_Write_UINT8(s, 1);                  /* CodecChannelT.codecId (1 byte) */
	Stream_Write_UINT8(s, 0);                  /* CodecChannelT.channelId (1 byte) */
	Stream_Write_UINT8(s, 1);                  /* regionFlags (1 byte) */
	Stream_Write_UINT16(s, message->numRects); /* numRects (2 bytes) */

	for (i = 0; i < message->numRects; i++)
	{
		const RFX_RECT* rect = rfx_message_get_rect(message, i);
		/* Clipping rectangles are relative to destLeft, destTop */
		Stream_Write_UINT16(s, rect->x);      /* x (2 bytes) */
		Stream_Write_UINT16(s, rect->y);      /* y (2 bytes) */
		Stream_Write_UINT16(s, rect->width);  /* width (2 bytes) */
		Stream_Write_UINT16(s, rect->height); /* height (2 bytes) */
	}

	Stream_Write_UINT16(s, CBT_REGION); /* regionType (2 bytes) */
	Stream_Write_UINT16(s, 1);          /* numTilesets (2 bytes) */
	return TRUE;
}