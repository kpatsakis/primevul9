static void rfx_write_message_codec_versions(RFX_CONTEXT* context, wStream* s)
{
	Stream_Write_UINT16(s, WBT_CODEC_VERSIONS); /* BlockT.blockType (2 bytes) */
	Stream_Write_UINT32(s, 10);                 /* BlockT.blockLen (4 bytes) */
	Stream_Write_UINT8(s, 1);                   /* numCodecs (1 byte) */
	Stream_Write_UINT8(s, 1);                   /* codecs.codecId (1 byte) */
	Stream_Write_UINT16(s, WF_VERSION_1_0);     /* codecs.version (2 bytes) */
}