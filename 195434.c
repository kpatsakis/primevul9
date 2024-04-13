static INLINE BOOL freerdp_bitmap_planar_compress_planes_rle(BYTE* inPlanes[4], UINT32 width,
                                                             UINT32 height, BYTE* outPlanes,
                                                             UINT32* dstSizes, BOOL skipAlpha)
{
	UINT32 outPlanesSize = width * height * 4;

	/* AlphaPlane */
	if (skipAlpha)
	{
		dstSizes[0] = 0;
	}
	else
	{
		dstSizes[0] = outPlanesSize;

		if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[0], width, height, outPlanes,
		                                              &dstSizes[0]))
			return FALSE;

		outPlanes += dstSizes[0];
		outPlanesSize -= dstSizes[0];
	}

	/* LumaOrRedPlane */
	dstSizes[1] = outPlanesSize;

	if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[1], width, height, outPlanes,
	                                              &dstSizes[1]))
		return FALSE;

	outPlanes += dstSizes[1];
	outPlanesSize -= dstSizes[1];
	/* OrangeChromaOrGreenPlane */
	dstSizes[2] = outPlanesSize;

	if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[2], width, height, outPlanes,
	                                              &dstSizes[2]))
		return FALSE;

	outPlanes += dstSizes[2];
	outPlanesSize -= dstSizes[2];
	/* GreenChromeOrBluePlane */
	dstSizes[3] = outPlanesSize;

	if (!freerdp_bitmap_planar_compress_plane_rle(inPlanes[3], width, height, outPlanes,
	                                              &dstSizes[3]))
		return FALSE;

	return TRUE;
}