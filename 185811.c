static void rfx_write_message_channels(RFX_CONTEXT* context, wStream* s)
{
	Stream_Write_UINT16(s, WBT_CHANNELS);    /* BlockT.blockType (2 bytes) */
	Stream_Write_UINT32(s, 12);              /* BlockT.blockLen (4 bytes) */
	Stream_Write_UINT8(s, 1);                /* numChannels (1 byte) */
	Stream_Write_UINT8(s, 0);                /* Channel.channelId (1 byte) */
	Stream_Write_UINT16(s, context->width);  /* Channel.width (2 bytes) */
	Stream_Write_UINT16(s, context->height); /* Channel.height (2 bytes) */
}