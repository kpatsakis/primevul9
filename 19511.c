static BOOL video_onMappedGeometryUpdate(MAPPED_GEOMETRY* geometry)
{
	PresentationContext* presentation = (PresentationContext*)geometry->custom;
	RDP_RECT* r = &geometry->geometry.boundingRect;
	WLog_DBG(TAG, "geometry updated topGeom=(%d,%d-%dx%d) geom=(%d,%d-%dx%d) rects=(%d,%d-%dx%d)",
	         geometry->topLevelLeft, geometry->topLevelTop,
	         geometry->topLevelRight - geometry->topLevelLeft,
	         geometry->topLevelBottom - geometry->topLevelTop,

	         geometry->left, geometry->top, geometry->right - geometry->left,
	         geometry->bottom - geometry->top,

	         r->x, r->y, r->width, r->height);

	presentation->surface->x = geometry->topLevelLeft + geometry->left;
	presentation->surface->y = geometry->topLevelTop + geometry->top;

	return TRUE;
}