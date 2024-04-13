static BOOL setupWorkers(RFX_CONTEXT* context, int nbTiles)
{
	RFX_CONTEXT_PRIV* priv = context->priv;
	void* pmem;

	if (!context->priv->UseThreads)
		return TRUE;

	if (!(pmem = realloc((void*)priv->workObjects, sizeof(PTP_WORK) * nbTiles)))
		return FALSE;

	priv->workObjects = (PTP_WORK*)pmem;

	if (!(pmem =
	          realloc((void*)priv->tileWorkParams, sizeof(RFX_TILE_COMPOSE_WORK_PARAM) * nbTiles)))
		return FALSE;

	priv->tileWorkParams = (RFX_TILE_COMPOSE_WORK_PARAM*)pmem;
	return TRUE;
}