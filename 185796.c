static BOOL rfx_process_message_codec_versions(RFX_CONTEXT* context, wStream* s)
{
	BYTE numCodecs;
	context->decodedHeaderBlocks &= ~_RFX_DECODED_VERSIONS;

	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "%s: packet too small for reading codec versions", __FUNCTION__);
		return FALSE;
	}

	Stream_Read_UINT8(s, numCodecs);         /* numCodecs (1 byte), must be set to 0x01 */
	Stream_Read_UINT8(s, context->codec_id); /* codecId (1 byte), must be set to 0x01 */
	Stream_Read_UINT16(
	    s, context->codec_version); /* version (2 bytes), must be set to WF_VERSION_1_0 (0x0100)  */

	if (numCodecs != 1)
	{
		WLog_ERR(TAG, "%s: numCodes is 0x%02" PRIX8 " (must be 0x01)", __FUNCTION__, numCodecs);
		return FALSE;
	}

	if (context->codec_id != 0x01)
	{
		WLog_ERR(TAG, "%s: invalid codec id (0x%02" PRIX32 ")", __FUNCTION__, context->codec_id);
		return FALSE;
	}

	if (context->codec_version != WF_VERSION_1_0)
	{
		WLog_ERR(TAG, "%s: invalid codec version (0x%08" PRIX32 ")", __FUNCTION__,
		         context->codec_version);
		return FALSE;
	}

	WLog_Print(context->priv->log, WLOG_DEBUG, "id %" PRIu32 " version 0x%" PRIX32 ".",
	           context->codec_id, context->codec_version);
	context->decodedHeaderBlocks |= _RFX_DECODED_VERSIONS;
	return TRUE;
}