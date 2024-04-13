static void rfx_tile_init(void* obj)
{
	RFX_TILE* tile = (RFX_TILE*)obj;
	if (tile)
	{
		tile->x = 0;
		tile->y = 0;
		tile->YLen = 0;
		tile->YData = NULL;
		tile->CbLen = 0;
		tile->CbData = NULL;
		tile->CrLen = 0;
		tile->CrData = NULL;
	}
}