static void VideoFrame_free(VideoFrame** pframe)
{
	VideoFrame* frame = *pframe;

	mappedGeometryUnref(frame->geometry);
	BufferPool_Return(frame->presentation->video->priv->surfacePool, frame->surfaceData);
	PresentationContext_unref(frame->presentation);
	free(frame);
	*pframe = NULL;
}