static void rfx_write_message_sync(RFX_CONTEXT* context, wStream* s)
{
	Stream_Write_UINT16(s, WBT_SYNC);       /* BlockT.blockType (2 bytes) */
	Stream_Write_UINT32(s, 12);             /* BlockT.blockLen (4 bytes) */
	Stream_Write_UINT32(s, WF_MAGIC);       /* magic (4 bytes) */
	Stream_Write_UINT16(s, WF_VERSION_1_0); /* version (2 bytes) */
}