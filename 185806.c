static RFX_MESSAGE* rfx_split_message(RFX_CONTEXT* context, RFX_MESSAGE* message, int* numMessages,
                                      int maxDataSize)
{
	int i, j;
	UINT32 tileDataSize;
	RFX_MESSAGE* messages;
	maxDataSize -= 1024; /* reserve enough space for headers */
	*numMessages = ((message->tilesDataSize + maxDataSize) / maxDataSize) * 4;

	if (!(messages = (RFX_MESSAGE*)calloc((*numMessages), sizeof(RFX_MESSAGE))))
		return NULL;

	j = 0;

	for (i = 0; i < message->numTiles; i++)
	{
		tileDataSize = rfx_tile_length(message->tiles[i]);

		if ((messages[j].tilesDataSize + tileDataSize) > ((UINT32)maxDataSize))
			j++;

		if (!messages[j].numTiles)
		{
			messages[j].frameIdx = message->frameIdx + j;
			messages[j].numQuant = message->numQuant;
			messages[j].quantVals = message->quantVals;
			messages[j].numRects = message->numRects;
			messages[j].rects = message->rects;
			messages[j].freeRects = FALSE;
			messages[j].freeArray = TRUE;

			if (!(messages[j].tiles = (RFX_TILE**)calloc(message->numTiles, sizeof(RFX_TILE*))))
				goto free_messages;
		}

		messages[j].tilesDataSize += tileDataSize;
		messages[j].tiles[messages[j].numTiles++] = message->tiles[i];
		message->tiles[i] = NULL;
	}

	*numMessages = j + 1;
	context->frameIdx += j;
	message->numTiles = 0;
	return messages;
free_messages:

	for (i = 0; i < j; i++)
		free(messages[i].tiles);

	free(messages);
	return NULL;
}