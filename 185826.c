static void* rfx_decoder_tile_new(void* val)
{
	RFX_TILE* tile = NULL;
	WINPR_UNUSED(val);

	if (!(tile = (RFX_TILE*)calloc(1, sizeof(RFX_TILE))))
		return NULL;

	if (!(tile->data = (BYTE*)_aligned_malloc(4 * 64 * 64, 16)))
	{
		free(tile);
		return NULL;
	}

	tile->allocated = TRUE;
	return tile;
}