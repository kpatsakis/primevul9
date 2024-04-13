static BOOL planar_subsample_expand(const BYTE* plane, size_t planeLength, UINT32 nWidth,
                                    UINT32 nHeight, UINT32 nPlaneWidth, UINT32 nPlaneHeight,
                                    BYTE* deltaPlane)
{
	size_t pos = 0;
	UINT32 y;
	if (!plane || !deltaPlane)
		return FALSE;

	if (nWidth > nPlaneWidth * 2)
		return FALSE;

	if (nHeight > nPlaneHeight * 2)
		return FALSE;

	for (y = 0; y < nHeight; y++)
	{
		const BYTE* src = plane + y / 2 * nPlaneWidth;
		UINT32 x;

		for (x = 0; x < nWidth; x++)
		{
			deltaPlane[pos++] = src[x / 2];
		}
	}

	return TRUE;
}