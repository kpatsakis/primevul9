static BOOL video_onMappedGeometryClear(MAPPED_GEOMETRY* geometry)
{
	PresentationContext* presentation = (PresentationContext*)geometry->custom;

	mappedGeometryUnref(presentation->geometry);
	presentation->geometry = NULL;
	return TRUE;
}