BOOL rfx_process_message(RFX_CONTEXT* context, const BYTE* data, UINT32 length, UINT32 left,
                         UINT32 top, BYTE* dst, UINT32 dstFormat, UINT32 dstStride,
                         UINT32 dstHeight, REGION16* invalidRegion)
{
	REGION16 updateRegion;
	UINT32 blockLen;
	UINT32 blockType;
	wStream inStream, *s = &inStream;
	BOOL ok = TRUE;
	RFX_MESSAGE* message;

	if (!context || !data || !length)
		return FALSE;

	message = &context->currentMessage;

	Stream_StaticInit(s, (BYTE*)data, length);

	message->freeRects = TRUE;

	while (ok && Stream_GetRemainingLength(s) > 6)
	{
		wStream subStream;
		size_t extraBlockLen = 0;

		/* RFX_BLOCKT */
		Stream_Read_UINT16(s, blockType); /* blockType (2 bytes) */
		Stream_Read_UINT32(s, blockLen);  /* blockLen (4 bytes) */
		WLog_Print(context->priv->log, WLOG_DEBUG, "blockType 0x%" PRIX32 " blockLen %" PRIu32 "",
		           blockType, blockLen);

		if (blockLen < 6)
		{
			WLog_ERR(TAG, "blockLen too small(%" PRIu32 ")", blockLen);
			return FALSE;
		}

		if (Stream_GetRemainingLength(s) < blockLen - 6)
		{
			WLog_ERR(TAG, "%s: packet too small for blocklen=%" PRIu32 "", __FUNCTION__, blockLen);
			return FALSE;
		}

		if (blockType > WBT_CONTEXT && context->decodedHeaderBlocks != _RFX_DECODED_HEADERS)
		{
			WLog_ERR(TAG, "%s: incomplete header blocks processing", __FUNCTION__);
			return FALSE;
		}

		if (blockType >= WBT_CONTEXT && blockType <= WBT_EXTENSION)
		{
			/* RFX_CODEC_CHANNELT */
			UINT8 codecId;
			UINT8 channelId;

			if (Stream_GetRemainingLength(s) < 2)
				return FALSE;

			extraBlockLen = 2;
			Stream_Read_UINT8(s, codecId);   /* codecId (1 byte) must be set to 0x01 */
			Stream_Read_UINT8(s, channelId); /* channelId (1 byte) 0xFF or 0x00, see below */

			if (codecId != 0x01)
			{
				WLog_ERR(TAG, "%s: invalid codecId 0x%02" PRIX8 "", __FUNCTION__, codecId);
				return FALSE;
			}

			if (blockType == WBT_CONTEXT)
			{
				/* If the blockType is set to WBT_CONTEXT, then channelId MUST be set to 0xFF.*/
				if (channelId != 0xFF)
				{
					WLog_ERR(TAG,
					         "%s: invalid channelId 0x%02" PRIX8 " for blockType 0x%08" PRIX32 "",
					         __FUNCTION__, channelId, blockType);
					return FALSE;
				}
			}
			else
			{
				/* For all other values of blockType, channelId MUST be set to 0x00. */
				if (channelId != 0x00)
				{
					WLog_ERR(TAG, "%s: invalid channelId 0x%02" PRIX8 " for blockType WBT_CONTEXT",
					         __FUNCTION__, channelId);
					return FALSE;
				}
			}
		}

		Stream_StaticInit(&subStream, Stream_Pointer(s), blockLen - (6 + extraBlockLen));
		Stream_Seek(s, blockLen - (6 + extraBlockLen));

		switch (blockType)
		{
			/* Header messages:
			 * The stream MUST start with the header messages and any of these headers can appear
			 * in the stream at a later stage. The header messages can be repeated.
			 */
			case WBT_SYNC:
				ok = rfx_process_message_sync(context, &subStream);
				break;

			case WBT_CONTEXT:
				ok = rfx_process_message_context(context, &subStream);
				break;

			case WBT_CODEC_VERSIONS:
				ok = rfx_process_message_codec_versions(context, &subStream);
				break;

			case WBT_CHANNELS:
				ok = rfx_process_message_channels(context, &subStream);
				break;

				/* Data messages:
				 * The data associated with each encoded frame or image is always bracketed by the
				 * TS_RFX_FRAME_BEGIN (section 2.2.2.3.1) and TS_RFX_FRAME_END (section 2.2.2.3.2)
				 * messages. There MUST only be one TS_RFX_REGION (section 2.2.2.3.3) message per
				 * frame and one TS_RFX_TILESET (section 2.2.2.3.4) message per TS_RFX_REGION.
				 */

			case WBT_FRAME_BEGIN:
				ok = rfx_process_message_frame_begin(context, message, &subStream,
				                                     &context->expectedDataBlockType);
				break;

			case WBT_REGION:
				ok = rfx_process_message_region(context, message, &subStream,
				                                &context->expectedDataBlockType);
				break;

			case WBT_EXTENSION:
				ok = rfx_process_message_tileset(context, message, &subStream,
				                                 &context->expectedDataBlockType);
				break;

			case WBT_FRAME_END:
				ok = rfx_process_message_frame_end(context, message, &subStream,
				                                   &context->expectedDataBlockType);
				break;

			default:
				WLog_ERR(TAG, "%s: unknown blockType 0x%" PRIX32 "", __FUNCTION__, blockType);
				return FALSE;
		}
	}

	if (ok)
	{
		UINT32 i, j;
		UINT32 nbUpdateRects;
		REGION16 clippingRects;
		const RECTANGLE_16* updateRects;
		const DWORD formatSize = GetBytesPerPixel(context->pixel_format);
		const UINT32 dstWidth = dstStride / GetBytesPerPixel(dstFormat);
		region16_init(&clippingRects);

		for (i = 0; i < message->numRects; i++)
		{
			RECTANGLE_16 clippingRect;
			const RFX_RECT* rect = &(message->rects[i]);
			clippingRect.left = MIN(left + rect->x, dstWidth);
			clippingRect.top = MIN(top + rect->y, dstHeight);
			clippingRect.right = MIN(clippingRect.left + rect->width, dstWidth);
			clippingRect.bottom = MIN(clippingRect.top + rect->height, dstHeight);
			region16_union_rect(&clippingRects, &clippingRects, &clippingRect);
		}

		for (i = 0; i < message->numTiles; i++)
		{
			RECTANGLE_16 updateRect;
			const RFX_TILE* tile = rfx_message_get_tile(message, i);
			updateRect.left = left + tile->x;
			updateRect.top = top + tile->y;
			updateRect.right = updateRect.left + 64;
			updateRect.bottom = updateRect.top + 64;
			region16_init(&updateRegion);
			region16_intersect_rect(&updateRegion, &clippingRects, &updateRect);
			updateRects = region16_rects(&updateRegion, &nbUpdateRects);

			for (j = 0; j < nbUpdateRects; j++)
			{
				const UINT32 stride = 64 * formatSize;
				const UINT32 nXDst = updateRects[j].left;
				const UINT32 nYDst = updateRects[j].top;
				const UINT32 nXSrc = nXDst - updateRect.left;
				const UINT32 nYSrc = nYDst - updateRect.top;
				const UINT32 nWidth = updateRects[j].right - updateRects[j].left;
				const UINT32 nHeight = updateRects[j].bottom - updateRects[j].top;

				if (!freerdp_image_copy(dst, dstFormat, dstStride, nXDst, nYDst, nWidth, nHeight,
				                        tile->data, context->pixel_format, stride, nXSrc, nYSrc,
				                        NULL, FREERDP_FLIP_NONE))
				{
					region16_uninit(&updateRegion);
					return FALSE;
				}

				if (invalidRegion)
					region16_union_rect(invalidRegion, invalidRegion, &updateRects[j]);
			}

			region16_uninit(&updateRegion);
		}

		region16_uninit(&clippingRects);
		return TRUE;
	}

	return FALSE;
}