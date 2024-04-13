static void* rfx_encoder_tile_new(void* val)
{
	WINPR_UNUSED(val);
	return calloc(1, sizeof(RFX_TILE));
}