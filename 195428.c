BOOL freerdp_bitmap_planar_context_reset(BITMAP_PLANAR_CONTEXT* context, UINT32 width,
                                         UINT32 height)
{
	if (!context)
		return FALSE;

	context->maxWidth = width;
	context->maxHeight = height;
	context->maxPlaneSize = context->maxWidth * context->maxHeight;
	context->nTempStep = context->maxWidth * 4;
	free(context->planesBuffer);
	free(context->pTempData);
	free(context->deltaPlanesBuffer);
	free(context->rlePlanesBuffer);
	context->planesBuffer = calloc(context->maxPlaneSize, 4);
	context->pTempData = calloc(context->maxPlaneSize, 6);
	context->deltaPlanesBuffer = calloc(context->maxPlaneSize, 4);
	context->rlePlanesBuffer = calloc(context->maxPlaneSize, 4);

	if (!context->planesBuffer || !context->pTempData || !context->deltaPlanesBuffer ||
	    !context->rlePlanesBuffer)
		return FALSE;

	context->planes[0] = &context->planesBuffer[context->maxPlaneSize * 0];
	context->planes[1] = &context->planesBuffer[context->maxPlaneSize * 1];
	context->planes[2] = &context->planesBuffer[context->maxPlaneSize * 2];
	context->planes[3] = &context->planesBuffer[context->maxPlaneSize * 3];
	context->deltaPlanes[0] = &context->deltaPlanesBuffer[context->maxPlaneSize * 0];
	context->deltaPlanes[1] = &context->deltaPlanesBuffer[context->maxPlaneSize * 1];
	context->deltaPlanes[2] = &context->deltaPlanesBuffer[context->maxPlaneSize * 2];
	context->deltaPlanes[3] = &context->deltaPlanesBuffer[context->maxPlaneSize * 3];
	return TRUE;
}