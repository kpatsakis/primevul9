void rfx_message_free(RFX_CONTEXT* context, RFX_MESSAGE* message)
{
	int i;
	RFX_TILE* tile;

	if (message)
	{
		if ((message->rects) && (message->freeRects))
		{
			free(message->rects);
		}

		if (message->tiles)
		{
			for (i = 0; i < message->numTiles; i++)
			{
				if (!(tile = message->tiles[i]))
					continue;

				if (tile->YCbCrData)
				{
					BufferPool_Return(context->priv->BufferPool, tile->YCbCrData);
					tile->YCbCrData = NULL;
				}

				ObjectPool_Return(context->priv->TilePool, (void*)tile);
			}

			free(message->tiles);
		}

		if (!message->freeArray)
			free(message);
	}
}