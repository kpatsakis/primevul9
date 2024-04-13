static BOOL rfx_process_message_context(RFX_CONTEXT* context, wStream* s)
{
	BYTE ctxId;
	UINT16 tileSize;
	UINT16 properties;
	context->decodedHeaderBlocks &= ~_RFX_DECODED_CONTEXT;

	if (Stream_GetRemainingLength(s) < 5)
	{
		WLog_ERR(TAG, "RfxMessageContext packet too small");
		return FALSE;
	}

	Stream_Read_UINT8(s, ctxId);     /* ctxId (1 byte), must be set to 0x00 */
	Stream_Read_UINT16(s, tileSize); /* tileSize (2 bytes), must be set to CT_TILE_64x64 (0x0040) */
	Stream_Read_UINT16(s, properties); /* properties (2 bytes) */
	WLog_Print(context->priv->log, WLOG_DEBUG,
	           "ctxId %" PRIu8 " tileSize %" PRIu16 " properties 0x%04" PRIX16 ".", ctxId, tileSize,
	           properties);
	context->properties = properties;
	context->flags = (properties & 0x0007);

	if (context->flags == CODEC_MODE)
	{
		WLog_Print(context->priv->log, WLOG_DEBUG, "codec is in image mode.");
	}
	else
	{
		WLog_Print(context->priv->log, WLOG_DEBUG, "codec is in video mode.");
	}

	switch ((properties & 0x1E00) >> 9)
	{
		case CLW_ENTROPY_RLGR1:
			context->mode = RLGR1;
			WLog_Print(context->priv->log, WLOG_DEBUG, "RLGR1.");
			break;

		case CLW_ENTROPY_RLGR3:
			context->mode = RLGR3;
			WLog_Print(context->priv->log, WLOG_DEBUG, "RLGR3.");
			break;

		default:
			WLog_ERR(TAG, "unknown RLGR algorithm.");
			return FALSE;
	}

	context->decodedHeaderBlocks |= _RFX_DECODED_CONTEXT;
	return TRUE;
}