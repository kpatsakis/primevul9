static BOOL rfx_process_message_frame_begin(RFX_CONTEXT* context, RFX_MESSAGE* message, wStream* s,
                                            UINT16* pExpectedBlockType)
{
	UINT32 frameIdx;
	UINT16 numRegions;

	if (*pExpectedBlockType != WBT_FRAME_BEGIN)
	{
		WLog_ERR(TAG, "%s: message unexpected wants WBT_FRAME_BEGIN", __FUNCTION__);
		return FALSE;
	}

	*pExpectedBlockType = WBT_REGION;

	if (Stream_GetRemainingLength(s) < 6)
	{
		WLog_ERR(TAG, "RfxMessageFrameBegin packet too small");
		return FALSE;
	}

	Stream_Read_UINT32(
	    s, frameIdx); /* frameIdx (4 bytes), if codec is in video mode, must be ignored */
	Stream_Read_UINT16(s, numRegions); /* numRegions (2 bytes) */
	WLog_Print(context->priv->log, WLOG_DEBUG,
	           "RFX_FRAME_BEGIN: frameIdx: %" PRIu32 " numRegions: %" PRIu16 "", frameIdx,
	           numRegions);
	return TRUE;
}