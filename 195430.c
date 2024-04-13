BOOL planar_decompress(BITMAP_PLANAR_CONTEXT* planar, const BYTE* pSrcData, UINT32 SrcSize,
                       UINT32 nSrcWidth, UINT32 nSrcHeight, BYTE* pDstData, UINT32 DstFormat,
                       UINT32 nDstStep, UINT32 nXDst, UINT32 nYDst, UINT32 nDstWidth,
                       UINT32 nDstHeight, BOOL vFlip)
{
	BOOL cs;
	BOOL rle;
	UINT32 cll;
	BOOL alpha;
	BOOL useAlpha = FALSE;
	INT32 status;
	const BYTE* srcp;
	UINT32 subSize;
	UINT32 subWidth;
	UINT32 subHeight;
	UINT32 planeSize;
	INT32 rleSizes[4] = { 0, 0, 0, 0 };
	UINT32 rawSizes[4];
	UINT32 rawWidths[4];
	UINT32 rawHeights[4];
	BYTE FormatHeader;
	const BYTE* planes[4] = { 0 };
	const UINT32 w = MIN(nSrcWidth, nDstWidth);
	const UINT32 h = MIN(nSrcHeight, nDstHeight);
	const primitives_t* prims = primitives_get();

	if (nDstStep <= 0)
		nDstStep = nDstWidth * GetBytesPerPixel(DstFormat);

	srcp = pSrcData;

	if (!pDstData)
	{
		WLog_ERR(TAG, "Invalid argument pDstData=NULL");
		return FALSE;
	}

	FormatHeader = *srcp++;
	cll = (FormatHeader & PLANAR_FORMAT_HEADER_CLL_MASK);
	cs = (FormatHeader & PLANAR_FORMAT_HEADER_CS) ? TRUE : FALSE;
	rle = (FormatHeader & PLANAR_FORMAT_HEADER_RLE) ? TRUE : FALSE;
	alpha = (FormatHeader & PLANAR_FORMAT_HEADER_NA) ? FALSE : TRUE;

	if (alpha)
		useAlpha = ColorHasAlpha(DstFormat);

	// WLog_INFO(TAG, "CLL: %"PRIu32" CS: %"PRIu8" RLE: %"PRIu8" ALPHA: %"PRIu8"", cll, cs, rle,
	// alpha);

	if (!cll && cs)
	{
		WLog_ERR(TAG, "Chroma subsampling requires YCoCg and does not work with RGB data");
		return FALSE; /* Chroma subsampling requires YCoCg */
	}

	subWidth = (nSrcWidth / 2) + (nSrcWidth % 2);
	subHeight = (nSrcHeight / 2) + (nSrcHeight % 2);
	planeSize = nSrcWidth * nSrcHeight;
	subSize = subWidth * subHeight;

	if (!cs)
	{
		rawSizes[0] = planeSize; /* LumaOrRedPlane */
		rawWidths[0] = nSrcWidth;
		rawHeights[0] = nSrcHeight;
		rawSizes[1] = planeSize; /* OrangeChromaOrGreenPlane */
		rawWidths[1] = nSrcWidth;
		rawHeights[1] = nSrcHeight;
		rawSizes[2] = planeSize; /* GreenChromaOrBluePlane */
		rawWidths[2] = nSrcWidth;
		rawHeights[2] = nSrcHeight;
		rawSizes[3] = planeSize; /* AlphaPlane */
		rawWidths[3] = nSrcWidth;
		rawHeights[3] = nSrcHeight;
	}
	else /* Chroma Subsampling */
	{
		rawSizes[0] = planeSize; /* LumaOrRedPlane */
		rawWidths[0] = nSrcWidth;
		rawHeights[0] = nSrcHeight;
		rawSizes[1] = subSize; /* OrangeChromaOrGreenPlane */
		rawWidths[1] = subWidth;
		rawHeights[1] = subHeight;
		rawSizes[2] = subSize; /* GreenChromaOrBluePlane */
		rawWidths[2] = subWidth;
		rawHeights[2] = subHeight;
		rawSizes[3] = planeSize; /* AlphaPlane */
		rawWidths[3] = nSrcWidth;
		rawHeights[3] = nSrcHeight;
	}

	if (!rle) /* RAW */
	{
		UINT32 base = planeSize * 3;
		if (cs)
			base = planeSize + planeSize / 2;

		if (alpha)
		{
			if ((SrcSize - (srcp - pSrcData)) < (planeSize + base))
				return FALSE;

			planes[3] = srcp;                    /* AlphaPlane */
			planes[0] = planes[3] + rawSizes[3]; /* LumaOrRedPlane */
			planes[1] = planes[0] + rawSizes[0]; /* OrangeChromaOrGreenPlane */
			planes[2] = planes[1] + rawSizes[1]; /* GreenChromaOrBluePlane */

			if ((planes[2] + rawSizes[2]) > &pSrcData[SrcSize])
				return FALSE;
		}
		else
		{
			if ((SrcSize - (srcp - pSrcData)) < base)
				return FALSE;

			planes[0] = srcp;                    /* LumaOrRedPlane */
			planes[1] = planes[0] + rawSizes[0]; /* OrangeChromaOrGreenPlane */
			planes[2] = planes[1] + rawSizes[1]; /* GreenChromaOrBluePlane */

			if ((planes[2] + rawSizes[2]) > &pSrcData[SrcSize])
				return FALSE;
		}
	}
	else /* RLE */
	{
		if (alpha)
		{
			planes[3] = srcp;
			rleSizes[3] = planar_skip_plane_rle(planes[3], SrcSize - (planes[3] - pSrcData),
			                                    rawWidths[3], rawHeights[3]); /* AlphaPlane */

			if (rleSizes[3] < 0)
				return FALSE;

			planes[0] = planes[3] + rleSizes[3];
		}
		else
			planes[0] = srcp;

		rleSizes[0] = planar_skip_plane_rle(planes[0], SrcSize - (planes[0] - pSrcData),
		                                    rawWidths[0], rawHeights[0]); /* RedPlane */

		if (rleSizes[0] < 0)
			return FALSE;

		planes[1] = planes[0] + rleSizes[0];
		rleSizes[1] = planar_skip_plane_rle(planes[1], SrcSize - (planes[1] - pSrcData),
		                                    rawWidths[1], rawHeights[1]); /* GreenPlane */

		if (rleSizes[1] < 1)
			return FALSE;

		planes[2] = planes[1] + rleSizes[1];
		rleSizes[2] = planar_skip_plane_rle(planes[2], SrcSize - (planes[2] - pSrcData),
		                                    rawWidths[2], rawHeights[2]); /* BluePlane */

		if (rleSizes[2] < 1)
			return FALSE;
	}

	if (!cll) /* RGB */
	{
		UINT32 TempFormat;
		BYTE* pTempData = pDstData;
		UINT32 nTempStep = nDstStep;

		if (useAlpha)
			TempFormat = PIXEL_FORMAT_BGRA32;
		else
			TempFormat = PIXEL_FORMAT_BGRX32;

		if ((TempFormat != DstFormat) || (nSrcWidth != nDstWidth) || (nSrcHeight != nDstHeight))
		{
			pTempData = planar->pTempData;
			nTempStep = planar->nTempStep;
		}

		if (!rle) /* RAW */
		{
			if (!planar_decompress_planes_raw(planes, pTempData, TempFormat, nTempStep, nXDst,
			                                  nYDst, nSrcWidth, nSrcHeight, vFlip))
				return FALSE;

			if (alpha)
				srcp += rawSizes[0] + rawSizes[1] + rawSizes[2] + rawSizes[3];
			else /* NoAlpha */
				srcp += rawSizes[0] + rawSizes[1] + rawSizes[2];

			if ((SrcSize - (srcp - pSrcData)) == 1)
				srcp++; /* pad */
		}
		else /* RLE */
		{
			status =
			    planar_decompress_plane_rle(planes[0], rleSizes[0], pTempData, nTempStep, nXDst,
			                                nYDst, nSrcWidth, nSrcHeight, 2, vFlip); /* RedPlane */

			if (status < 0)
				return FALSE;

			status = planar_decompress_plane_rle(planes[1], rleSizes[1], pTempData, nTempStep,
			                                     nXDst, nYDst, nSrcWidth, nSrcHeight, 1,
			                                     vFlip); /* GreenPlane */

			if (status < 0)
				return FALSE;

			status =
			    planar_decompress_plane_rle(planes[2], rleSizes[2], pTempData, nTempStep, nXDst,
			                                nYDst, nSrcWidth, nSrcHeight, 0, vFlip); /* BluePlane */

			if (status < 0)
				return FALSE;

			srcp += rleSizes[0] + rleSizes[1] + rleSizes[2];

			if (useAlpha)
			{
				status = planar_decompress_plane_rle(planes[3], rleSizes[3], pTempData, nTempStep,
				                                     nXDst, nYDst, nSrcWidth, nSrcHeight, 3,
				                                     vFlip); /* AlphaPlane */
			}
			else
				status = planar_set_plane(0xFF, pTempData, nTempStep, nXDst, nYDst, nSrcWidth,
				                          nSrcHeight, 3, vFlip);

			if (status < 0)
				return FALSE;

			if (alpha)
				srcp += rleSizes[3];
		}

		if (pTempData != pDstData)
		{
			if (!freerdp_image_copy(pDstData, DstFormat, nDstStep, nXDst, nYDst, w, h, pTempData,
			                        TempFormat, nTempStep, nXDst, nYDst, NULL, FREERDP_FLIP_NONE))
				return FALSE;
		}
	}
	else /* YCoCg */
	{
		UINT32 TempFormat;
		BYTE* pTempData = planar->pTempData;
		UINT32 nTempStep = planar->nTempStep;

		if (useAlpha)
			TempFormat = PIXEL_FORMAT_BGRA32;
		else
			TempFormat = PIXEL_FORMAT_BGRX32;

		if (!pTempData)
			return FALSE;

		if (rle) /* RLE encoded data. Decode and handle it like raw data. */
		{
			BYTE* rleBuffer[4] = { 0 };

			rleBuffer[3] = planar->rlePlanesBuffer;  /* AlphaPlane */
			rleBuffer[0] = rleBuffer[3] + planeSize; /* LumaOrRedPlane */
			rleBuffer[1] = rleBuffer[0] + planeSize; /* OrangeChromaOrGreenPlane */
			rleBuffer[2] = rleBuffer[1] + planeSize; /* GreenChromaOrBluePlane */
			if (useAlpha)
			{
				status =
				    planar_decompress_plane_rle_only(planes[3], rleSizes[3], rleBuffer[3],
				                                     rawWidths[3], rawHeights[3]); /* AlphaPlane */

				if (status < 0)
					return FALSE;
			}

			if (alpha)
				srcp += rleSizes[3];

			status = planar_decompress_plane_rle_only(planes[0], rleSizes[0], rleBuffer[0],
			                                          rawWidths[0], rawHeights[0]); /* LumaPlane */

			if (status < 0)
				return FALSE;

			status =
			    planar_decompress_plane_rle_only(planes[1], rleSizes[1], rleBuffer[1], rawWidths[1],
			                                     rawHeights[1]); /* OrangeChromaPlane */

			if (status < 0)
				return FALSE;

			status =
			    planar_decompress_plane_rle_only(planes[2], rleSizes[2], rleBuffer[2], rawWidths[2],
			                                     rawHeights[2]); /* GreenChromaPlane */

			if (status < 0)
				return FALSE;

			planes[0] = rleBuffer[0];
			planes[1] = rleBuffer[1];
			planes[2] = rleBuffer[2];
			planes[3] = rleBuffer[3];
		}

		/* RAW */
		{
			if (cs)
			{ /* Chroma subsampling for Co and Cg:
			   * Each pixel contains the value that should be expanded to
			   * [2x,2y;2x+1,2y;2x+1,2y+1;2x;2y+1] */
				if (!planar_subsample_expand(planes[1], rawSizes[1], nSrcWidth, nSrcHeight,
				                             rawWidths[1], rawHeights[1], planar->deltaPlanes[0]))
					return FALSE;

				planes[1] = planar->deltaPlanes[0];
				rawSizes[1] = planeSize; /* OrangeChromaOrGreenPlane */
				rawWidths[1] = nSrcWidth;
				rawHeights[1] = nSrcHeight;

				if (!planar_subsample_expand(planes[2], rawSizes[2], nSrcWidth, nSrcHeight,
				                             rawWidths[2], rawHeights[2], planar->deltaPlanes[1]))
					return FALSE;

				planes[2] = planar->deltaPlanes[1];
				rawSizes[2] = planeSize; /* GreenChromaOrBluePlane */
				rawWidths[2] = nSrcWidth;
				rawHeights[2] = nSrcHeight;
			}

			if (!planar_decompress_planes_raw(planes, pTempData, TempFormat, nTempStep, nXDst,
			                                  nYDst, nSrcWidth, nSrcHeight, vFlip))
				return FALSE;

			if (alpha)
				srcp += rawSizes[0] + rawSizes[1] + rawSizes[2] + rawSizes[3];
			else /* NoAlpha */
				srcp += rawSizes[0] + rawSizes[1] + rawSizes[2];

			if ((SrcSize - (srcp - pSrcData)) == 1)
				srcp++; /* pad */
		}

		if (prims->YCoCgToRGB_8u_AC4R(pTempData, nTempStep, pDstData, DstFormat, nDstStep, w, h,
		                              cll, useAlpha) != PRIMITIVES_SUCCESS)
			return FALSE;
	}

	return TRUE;
}