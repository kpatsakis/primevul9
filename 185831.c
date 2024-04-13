static BOOL rfx_write_message_tileset(RFX_CONTEXT* context, wStream* s, RFX_MESSAGE* message)
{
	int i;
	RFX_TILE* tile;
	UINT32 blockLen;
	UINT32* quantVals;
	blockLen = 22 + (message->numQuant * 5) + message->tilesDataSize;

	if (!Stream_EnsureRemainingCapacity(s, blockLen))
		return FALSE;

	Stream_Write_UINT16(s, WBT_EXTENSION);          /* CodecChannelT.blockType (2 bytes) */
	Stream_Write_UINT32(s, blockLen);               /* set CodecChannelT.blockLen (4 bytes) */
	Stream_Write_UINT8(s, 1);                       /* CodecChannelT.codecId (1 byte) */
	Stream_Write_UINT8(s, 0);                       /* CodecChannelT.channelId (1 byte) */
	Stream_Write_UINT16(s, CBT_TILESET);            /* subtype (2 bytes) */
	Stream_Write_UINT16(s, 0);                      /* idx (2 bytes) */
	Stream_Write_UINT16(s, context->properties);    /* properties (2 bytes) */
	Stream_Write_UINT8(s, message->numQuant);       /* numQuant (1 byte) */
	Stream_Write_UINT8(s, 0x40);                    /* tileSize (1 byte) */
	Stream_Write_UINT16(s, message->numTiles);      /* numTiles (2 bytes) */
	Stream_Write_UINT32(s, message->tilesDataSize); /* tilesDataSize (4 bytes) */
	quantVals = message->quantVals;

	for (i = 0; i < message->numQuant * 5; i++)
	{
		Stream_Write_UINT8(s, quantVals[0] + (quantVals[1] << 4));
		quantVals += 2;
	}

	for (i = 0; i < message->numTiles; i++)
	{
		if (!(tile = message->tiles[i]))
			return FALSE;

		if (!rfx_write_tile(context, s, tile))
			return FALSE;
	}

#ifdef WITH_DEBUG_RFX
	WLog_Print(context->priv->log, WLOG_DEBUG,
	           "numQuant: %" PRIu16 " numTiles: %" PRIu16 " tilesDataSize: %" PRIu32 "",
	           message->numQuant, message->numTiles, message->tilesDataSize);
#endif
	return TRUE;
}