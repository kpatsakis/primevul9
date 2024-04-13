RFX_MESSAGE* rfx_encode_message(RFX_CONTEXT* context, const RFX_RECT* rects, int numRects,
                                BYTE* data, int w, int h, int s)
{
	const UINT32 width = (UINT32)w;
	const UINT32 height = (UINT32)h;
	const UINT32 scanline = (UINT32)s;
	UINT32 i, maxNbTiles, maxTilesX, maxTilesY;
	UINT32 xIdx, yIdx, regionNbRects;
	UINT32 gridRelX, gridRelY, ax, ay, bytesPerPixel;
	RFX_TILE* tile;
	RFX_RECT* rfxRect;
	RFX_MESSAGE* message = NULL;
	PTP_WORK* workObject = NULL;
	RFX_TILE_COMPOSE_WORK_PARAM* workParam = NULL;
	BOOL success = FALSE;
	REGION16 rectsRegion, tilesRegion;
	RECTANGLE_16 currentTileRect;
	const RECTANGLE_16* regionRect;
	const RECTANGLE_16* extents;
	assert(data);
	assert(rects);
	assert(numRects > 0);
	assert(w > 0);
	assert(h > 0);
	assert(s > 0);

	if (!(message = (RFX_MESSAGE*)calloc(1, sizeof(RFX_MESSAGE))))
		return NULL;

	region16_init(&tilesRegion);
	region16_init(&rectsRegion);

	if (context->state == RFX_STATE_SEND_HEADERS)
		rfx_update_context_properties(context);

	message->frameIdx = context->frameIdx++;

	if (!context->numQuant)
	{
		if (!(context->quants = (UINT32*)malloc(sizeof(rfx_default_quantization_values))))
			goto skip_encoding_loop;

		CopyMemory(context->quants, &rfx_default_quantization_values,
		           sizeof(rfx_default_quantization_values));
		context->numQuant = 1;
		context->quantIdxY = 0;
		context->quantIdxCb = 0;
		context->quantIdxCr = 0;
	}

	message->numQuant = context->numQuant;
	message->quantVals = context->quants;
	bytesPerPixel = (context->bits_per_pixel / 8);

	if (!computeRegion(rects, numRects, &rectsRegion, width, height))
		goto skip_encoding_loop;

	extents = region16_extents(&rectsRegion);
	assert(extents->right - extents->left > 0);
	assert(extents->bottom - extents->top > 0);
	maxTilesX = 1 + TILE_NO(extents->right - 1) - TILE_NO(extents->left);
	maxTilesY = 1 + TILE_NO(extents->bottom - 1) - TILE_NO(extents->top);
	maxNbTiles = maxTilesX * maxTilesY;

	if (!(message->tiles = calloc(maxNbTiles, sizeof(RFX_TILE*))))
		goto skip_encoding_loop;

	if (!setupWorkers(context, maxNbTiles))
		goto skip_encoding_loop;

	if (context->priv->UseThreads)
	{
		workObject = context->priv->workObjects;
		workParam = context->priv->tileWorkParams;
	}

	regionRect = region16_rects(&rectsRegion, &regionNbRects);

	if (!(message->rects = calloc(regionNbRects, sizeof(RFX_RECT))))
		goto skip_encoding_loop;

	message->numRects = regionNbRects;

	for (i = 0, rfxRect = message->rects; i < regionNbRects; i++, regionRect++, rfxRect++)
	{
		UINT32 startTileX = regionRect->left / 64;
		UINT32 endTileX = (regionRect->right - 1) / 64;
		UINT32 startTileY = regionRect->top / 64;
		UINT32 endTileY = (regionRect->bottom - 1) / 64;
		rfxRect->x = regionRect->left;
		rfxRect->y = regionRect->top;
		rfxRect->width = (regionRect->right - regionRect->left);
		rfxRect->height = (regionRect->bottom - regionRect->top);

		for (yIdx = startTileY, gridRelY = startTileY * 64; yIdx <= endTileY;
		     yIdx++, gridRelY += 64)
		{
			UINT32 tileHeight = 64;

			if ((yIdx == endTileY) && (gridRelY + 64 > height))
				tileHeight = height - gridRelY;

			currentTileRect.top = gridRelY;
			currentTileRect.bottom = gridRelY + tileHeight;

			for (xIdx = startTileX, gridRelX = startTileX * 64; xIdx <= endTileX;
			     xIdx++, gridRelX += 64)
			{
				int tileWidth = 64;

				if ((xIdx == endTileX) && (gridRelX + 64 > width))
					tileWidth = width - gridRelX;

				currentTileRect.left = gridRelX;
				currentTileRect.right = gridRelX + tileWidth;

				/* checks if this tile is already treated */
				if (region16_intersects_rect(&tilesRegion, &currentTileRect))
					continue;

				if (!(tile = (RFX_TILE*)ObjectPool_Take(context->priv->TilePool)))
					goto skip_encoding_loop;

				tile->xIdx = xIdx;
				tile->yIdx = yIdx;
				tile->x = gridRelX;
				tile->y = gridRelY;
				tile->scanline = scanline;
				tile->width = tileWidth;
				tile->height = tileHeight;
				ax = gridRelX;
				ay = gridRelY;

				if (tile->data && tile->allocated)
				{
					free(tile->data);
					tile->allocated = FALSE;
				}

				tile->data = &data[(ay * scanline) + (ax * bytesPerPixel)];
				tile->quantIdxY = context->quantIdxY;
				tile->quantIdxCb = context->quantIdxCb;
				tile->quantIdxCr = context->quantIdxCr;
				tile->YLen = tile->CbLen = tile->CrLen = 0;

				if (!(tile->YCbCrData = (BYTE*)BufferPool_Take(context->priv->BufferPool, -1)))
					goto skip_encoding_loop;

				tile->YData = (BYTE*)&(tile->YCbCrData[((8192 + 32) * 0) + 16]);
				tile->CbData = (BYTE*)&(tile->YCbCrData[((8192 + 32) * 1) + 16]);
				tile->CrData = (BYTE*)&(tile->YCbCrData[((8192 + 32) * 2) + 16]);
				message->tiles[message->numTiles] = tile;
				message->numTiles++;

				if (context->priv->UseThreads)
				{
					workParam->context = context;
					workParam->tile = tile;

					if (!(*workObject = CreateThreadpoolWork(rfx_compose_message_tile_work_callback,
					                                         (void*)workParam,
					                                         &context->priv->ThreadPoolEnv)))
					{
						goto skip_encoding_loop;
					}

					SubmitThreadpoolWork(*workObject);
					workObject++;
					workParam++;
				}
				else
				{
					rfx_encode_rgb(context, tile);
				}

				if (!region16_union_rect(&tilesRegion, &tilesRegion, &currentTileRect))
					goto skip_encoding_loop;
			} /* xIdx */
		}     /* yIdx */
	}         /* rects */

	success = TRUE;
skip_encoding_loop:

	if (success && message->numTiles != maxNbTiles)
	{
		if (message->numTiles > 0)
		{
			void* pmem = realloc((void*)message->tiles, sizeof(RFX_TILE*) * message->numTiles);

			if (pmem)
				message->tiles = (RFX_TILE**)pmem;
			else
				success = FALSE;
		}
		else
			success = FALSE;
	}

	/* when using threads ensure all computations are done */
	if (success)
	{
		message->tilesDataSize = 0;
		workObject = context->priv->workObjects;

		for (i = 0; i < message->numTiles; i++)
		{
			tile = message->tiles[i];

			if (context->priv->UseThreads)
			{
				if (*workObject)
				{
					WaitForThreadpoolWorkCallbacks(*workObject, FALSE);
					CloseThreadpoolWork(*workObject);
				}

				workObject++;
			}

			message->tilesDataSize += rfx_tile_length(tile);
		}

		region16_uninit(&tilesRegion);
		region16_uninit(&rectsRegion);

		return message;
	}

	WLog_ERR(TAG, "%s: failed", __FUNCTION__);
	message->freeRects = TRUE;
	rfx_message_free(context, message);
	return NULL;
}