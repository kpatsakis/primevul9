static BOOL rfx_write_tile(RFX_CONTEXT* context, wStream* s, RFX_TILE* tile)
{
	UINT32 blockLen;
	blockLen = rfx_tile_length(tile);

	if (!Stream_EnsureRemainingCapacity(s, blockLen))
		return FALSE;

	Stream_Write_UINT16(s, CBT_TILE);           /* BlockT.blockType (2 bytes) */
	Stream_Write_UINT32(s, blockLen);           /* BlockT.blockLen (4 bytes) */
	Stream_Write_UINT8(s, tile->quantIdxY);     /* quantIdxY (1 byte) */
	Stream_Write_UINT8(s, tile->quantIdxCb);    /* quantIdxCb (1 byte) */
	Stream_Write_UINT8(s, tile->quantIdxCr);    /* quantIdxCr (1 byte) */
	Stream_Write_UINT16(s, tile->xIdx);         /* xIdx (2 bytes) */
	Stream_Write_UINT16(s, tile->yIdx);         /* yIdx (2 bytes) */
	Stream_Write_UINT16(s, tile->YLen);         /* YLen (2 bytes) */
	Stream_Write_UINT16(s, tile->CbLen);        /* CbLen (2 bytes) */
	Stream_Write_UINT16(s, tile->CrLen);        /* CrLen (2 bytes) */
	Stream_Write(s, tile->YData, tile->YLen);   /* YData */
	Stream_Write(s, tile->CbData, tile->CbLen); /* CbData */
	Stream_Write(s, tile->CrData, tile->CrLen); /* CrData */
	return TRUE;
}