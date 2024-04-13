static void rfx_write_message_context(RFX_CONTEXT* context, wStream* s)
{
	UINT16 properties;
	Stream_Write_UINT16(s, WBT_CONTEXT);   /* CodecChannelT.blockType (2 bytes) */
	Stream_Write_UINT32(s, 13);            /* CodecChannelT.blockLen (4 bytes) */
	Stream_Write_UINT8(s, 1);              /* CodecChannelT.codecId (1 byte) */
	Stream_Write_UINT8(s, 0xFF);           /* CodecChannelT.channelId (1 byte) */
	Stream_Write_UINT8(s, 0);              /* ctxId (1 byte) */
	Stream_Write_UINT16(s, CT_TILE_64x64); /* tileSize (2 bytes) */
	/* properties */
	properties = context->flags;             /* flags */
	properties |= (COL_CONV_ICT << 3);       /* cct */
	properties |= (CLW_XFORM_DWT_53_A << 5); /* xft */
	properties |= ((context->mode == RLGR1 ? CLW_ENTROPY_RLGR1 : CLW_ENTROPY_RLGR3) << 9); /* et */
	properties |= (SCALAR_QUANTIZATION << 13);                                             /* qt */
	Stream_Write_UINT16(s, properties); /* properties (2 bytes) */
	rfx_update_context_properties(context);
}