static BOOL rfx_process_message_region(RFX_CONTEXT* context, RFX_MESSAGE* message, wStream* s,
                                       UINT16* pExpectedBlockType)
{
	int i;
	UINT16 regionType;
	UINT16 numTileSets;
	RFX_RECT* tmpRects;

	if (*pExpectedBlockType != WBT_REGION)
	{
		WLog_ERR(TAG, "%s: message unexpected wants WBT_REGION", __FUNCTION__);
		return FALSE;
	}

	*pExpectedBlockType = WBT_EXTENSION;

	if (Stream_GetRemainingLength(s) < 3)
	{
		WLog_ERR(TAG, "%s: packet too small (regionFlags/numRects)", __FUNCTION__);
		return FALSE;
	}

	Stream_Seek_UINT8(s);                     /* regionFlags (1 byte) */
	Stream_Read_UINT16(s, message->numRects); /* numRects (2 bytes) */

	if (message->numRects < 1)
	{
		/*
		   If numRects is zero the decoder must generate a rectangle with
		   coordinates (0, 0, width, height).
		   See [MS-RDPRFX] (revision >= 17.0) 2.2.2.3.3 TS_RFX_REGION
		   https://msdn.microsoft.com/en-us/library/ff635233.aspx
		*/
		tmpRects = realloc(message->rects, sizeof(RFX_RECT));
		if (!tmpRects)
			return FALSE;

		message->numRects = 1;
		message->rects = tmpRects;
		message->rects->x = 0;
		message->rects->y = 0;
		message->rects->width = context->width;
		message->rects->height = context->height;
		return TRUE;
	}

	if (Stream_GetRemainingLength(s) < (size_t)(8 * message->numRects))
	{
		WLog_ERR(TAG, "%s: packet too small for num_rects=%" PRIu16 "", __FUNCTION__,
		         message->numRects);
		return FALSE;
	}

	tmpRects = realloc(message->rects, message->numRects * sizeof(RFX_RECT));
	if (!tmpRects)
		return FALSE;
	message->rects = tmpRects;

	/* rects */
	for (i = 0; i < message->numRects; i++)
	{
		RFX_RECT* rect = rfx_message_get_rect(message, i);
		/* RFX_RECT */
		Stream_Read_UINT16(s, rect->x);      /* x (2 bytes) */
		Stream_Read_UINT16(s, rect->y);      /* y (2 bytes) */
		Stream_Read_UINT16(s, rect->width);  /* width (2 bytes) */
		Stream_Read_UINT16(s, rect->height); /* height (2 bytes) */
		WLog_Print(context->priv->log, WLOG_DEBUG,
		           "rect %d (x,y=%" PRIu16 ",%" PRIu16 " w,h=%" PRIu16 " %" PRIu16 ").", i, rect->x,
		           rect->y, rect->width, rect->height);
	}

	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "%s: packet too small (regionType/numTileSets)", __FUNCTION__);
		return FALSE;
	}

	Stream_Read_UINT16(s, regionType);  /*regionType (2 bytes): MUST be set to CBT_REGION (0xCAC1)*/
	Stream_Read_UINT16(s, numTileSets); /*numTilesets (2 bytes): MUST be set to 0x0001.*/

	if (regionType != CBT_REGION)
	{
		WLog_ERR(TAG, "%s: invalid region type 0x%04" PRIX16 "", __FUNCTION__, regionType);
		return TRUE;
	}

	if (numTileSets != 0x0001)
	{
		WLog_ERR(TAG, "%s: invalid number of tilesets (%" PRIu16 ")", __FUNCTION__, numTileSets);
		return FALSE;
	}

	return TRUE;
}