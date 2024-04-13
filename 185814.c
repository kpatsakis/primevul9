static BOOL rfx_write_message_frame_begin(RFX_CONTEXT* context, wStream* s, RFX_MESSAGE* message)
{
	if (!Stream_EnsureRemainingCapacity(s, 14))
		return FALSE;

	Stream_Write_UINT16(s, WBT_FRAME_BEGIN);   /* CodecChannelT.blockType */
	Stream_Write_UINT32(s, 14);                /* CodecChannelT.blockLen */
	Stream_Write_UINT8(s, 1);                  /* CodecChannelT.codecId */
	Stream_Write_UINT8(s, 0);                  /* CodecChannelT.channelId */
	Stream_Write_UINT32(s, message->frameIdx); /* frameIdx */
	Stream_Write_UINT16(s, 1);                 /* numRegions */
	return TRUE;
}