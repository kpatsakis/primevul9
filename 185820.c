static BOOL rfx_process_message_frame_end(RFX_CONTEXT* context, RFX_MESSAGE* message, wStream* s,
                                          UINT16* pExpectedBlockType)
{
	if (*pExpectedBlockType != WBT_FRAME_END)
	{
		WLog_ERR(TAG, "%s: message unexpected, wants WBT_FRAME_END", __FUNCTION__);
		return FALSE;
	}

	*pExpectedBlockType = WBT_FRAME_BEGIN;
	WLog_Print(context->priv->log, WLOG_DEBUG, "RFX_FRAME_END");
	return TRUE;
}