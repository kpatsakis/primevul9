static BOOL computeRegion(const RFX_RECT* rects, int numRects, REGION16* region, int width,
                          int height)
{
	int i;
	const RFX_RECT* rect = rects;
	const RECTANGLE_16 mainRect = { 0, 0, width, height };

	for (i = 0; i < numRects; i++, rect++)
	{
		RECTANGLE_16 rect16;
		rect16.left = rect->x;
		rect16.top = rect->y;
		rect16.right = rect->x + rect->width;
		rect16.bottom = rect->y + rect->height;

		if (!region16_union_rect(region, region, &rect16))
			return FALSE;
	}

	return region16_intersect_rect(region, region, &mainRect);
}