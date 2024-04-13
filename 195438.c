static INLINE INT32 planar_set_plane(BYTE bValue, BYTE* pDstData, INT32 nDstStep, UINT32 nXDst,
                                     UINT32 nYDst, UINT32 nWidth, UINT32 nHeight, UINT32 nChannel,
                                     BOOL vFlip)
{
	INT32 x, y;
	INT32 beg, end, inc;

	if ((nHeight > INT32_MAX) || (nWidth > INT32_MAX) || (nDstStep > INT32_MAX))
		return -1;

	if (vFlip)
	{
		beg = (INT32)nHeight - 1;
		end = -1;
		inc = -1;
	}
	else
	{
		beg = 0;
		end = (INT32)nHeight;
		inc = 1;
	}

	for (y = beg; y != end; y += inc)
	{
		BYTE* dstp = &pDstData[((nYDst + y) * (INT32)nDstStep) + (nXDst * 4) + nChannel];

		for (x = 0; x < (INT32)nWidth; ++x)
		{
			*dstp = bValue;
			dstp += 4;
		}
	}

	return 0;
}