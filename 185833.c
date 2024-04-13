static void rfx_decoder_tile_free(void* obj)
{
	RFX_TILE* tile = (RFX_TILE*)obj;

	if (tile)
	{
		if (tile->allocated)
			_aligned_free(tile->data);

		free(tile);
	}
}