static BOOL rfx_process_message_sync(RFX_CONTEXT* context, wStream* s)
{
	UINT32 magic;
	context->decodedHeaderBlocks &= ~_RFX_DECODED_SYNC;

	/* RFX_SYNC */
	if (Stream_GetRemainingLength(s) < 6)
	{
		WLog_ERR(TAG, "RfxSync packet too small");
		return FALSE;
	}

	Stream_Read_UINT32(s, magic); /* magic (4 bytes), 0xCACCACCA */
	if (magic != WF_MAGIC)
	{
		WLog_ERR(TAG, "invalid magic number 0x%08" PRIX32 "", magic);
		return FALSE;
	}

	Stream_Read_UINT16(s, context->version); /* version (2 bytes), WF_VERSION_1_0 (0x0100) */
	if (context->version != WF_VERSION_1_0)
	{
		WLog_ERR(TAG, "invalid version number 0x%08" PRIX32 "", context->version);
		return FALSE;
	}

	WLog_Print(context->priv->log, WLOG_DEBUG, "version 0x%08" PRIX32 "", context->version);
	context->decodedHeaderBlocks |= _RFX_DECODED_SYNC;
	return TRUE;
}