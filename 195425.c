void freerdp_bitmap_planar_context_free(BITMAP_PLANAR_CONTEXT* context)
{
	if (!context)
		return;

	free(context->pTempData);
	free(context->planesBuffer);
	free(context->deltaPlanesBuffer);
	free(context->rlePlanesBuffer);
	free(context);
}