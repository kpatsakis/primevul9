static void PresentationContext_unref(PresentationContext* presentation)
{
	VideoClientContextPriv* priv;
	MAPPED_GEOMETRY* geometry;

	if (!presentation)
		return;

	if (InterlockedDecrement(&presentation->refCounter) != 0)
		return;

	geometry = presentation->geometry;
	if (geometry)
	{
		geometry->MappedGeometryUpdate = NULL;
		geometry->MappedGeometryClear = NULL;
		geometry->custom = NULL;
		mappedGeometryUnref(geometry);
	}

	priv = presentation->video->priv;

	h264_context_free(presentation->h264);
	Stream_Free(presentation->currentSample, TRUE);
	presentation->video->deleteSurface(presentation->video, presentation->surface);
	BufferPool_Return(priv->surfacePool, presentation->surfaceData);
	yuv_context_free(presentation->yuv);
	free(presentation);
}