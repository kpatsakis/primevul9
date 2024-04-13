static int rfx_tile_length(RFX_TILE* tile)
{
	return 19 + tile->YLen + tile->CbLen + tile->CrLen;
}