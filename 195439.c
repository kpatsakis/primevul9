static INLINE BOOL planar_decompress_planes_raw(const BYTE* pSrcData[4], BYTE* pDstData,
                                                UINT32 DstFormat, UINT32 nDstStep, UINT32 nXDst,
                                                UINT32 nYDst, UINT32 nWidth, UINT32 nHeight,
                                                BOOL vFlip)
{
	INT32 y;
	INT32 beg, end, inc;
	const BYTE* pR = pSrcData[0];
	const BYTE* pG = pSrcData[1];
	const BYTE* pB = pSrcData[2];
	const BYTE* pA = pSrcData[3];

	if (vFlip)
	{
		beg = nHeight - 1;
		end = -1;
		inc = -1;
	}
	else
	{
		beg = 0;
		end = nHeight;
		inc = 1;
	}

	for (y = beg; y != end; y += inc)
	{
		BYTE* pRGB = &pDstData[((nYDst + y) * nDstStep) + (nXDst * GetBytesPerPixel(DstFormat))];

		if (!writeLine(&pRGB, DstFormat, nWidth, &pR, &pG, &pB, &pA))
			return FALSE;
	}

	return TRUE;
}