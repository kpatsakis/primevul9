static INLINE INT32 planar_decompress_plane_rle_only(const BYTE* pSrcData, UINT32 SrcSize,
                                                     BYTE* pDstData, UINT32 nWidth, UINT32 nHeight)
{
	INT32 x, y;
	UINT32 pixel;
	UINT32 cRawBytes;
	UINT32 nRunLength;
	INT32 deltaValue;
	BYTE controlByte;
	BYTE* currentScanline;
	BYTE* previousScanline;
	const BYTE* srcp = pSrcData;

	if ((nHeight > INT32_MAX) || (nWidth > INT32_MAX))
		return -1;

	previousScanline = NULL;

	for (y = 0; y < (INT32)nHeight; y++)
	{
		BYTE* dstp = &pDstData[((y) * (INT32)nWidth)];
		pixel = 0;
		currentScanline = dstp;

		for (x = 0; x < (INT32)nWidth;)
		{
			controlByte = *srcp;
			srcp++;

			if ((srcp - pSrcData) > SrcSize)
			{
				WLog_ERR(TAG, "error reading input buffer");
				return -1;
			}

			nRunLength = PLANAR_CONTROL_BYTE_RUN_LENGTH(controlByte);
			cRawBytes = PLANAR_CONTROL_BYTE_RAW_BYTES(controlByte);

			if (nRunLength == 1)
			{
				nRunLength = cRawBytes + 16;
				cRawBytes = 0;
			}
			else if (nRunLength == 2)
			{
				nRunLength = cRawBytes + 32;
				cRawBytes = 0;
			}

			if (((dstp + (cRawBytes + nRunLength)) - currentScanline) > nWidth)
			{
				WLog_ERR(TAG, "too many pixels in scanline");
				return -1;
			}

			if (!previousScanline)
			{
				/* first scanline, absolute values */
				while (cRawBytes > 0)
				{
					pixel = *srcp;
					srcp++;
					*dstp = pixel;
					dstp++;
					x++;
					cRawBytes--;
				}

				while (nRunLength > 0)
				{
					*dstp = pixel;
					dstp++;
					x++;
					nRunLength--;
				}
			}
			else
			{
				/* delta values relative to previous scanline */
				while (cRawBytes > 0)
				{
					deltaValue = *srcp;
					srcp++;

					if (deltaValue & 1)
					{
						deltaValue = deltaValue >> 1;
						deltaValue = deltaValue + 1;
						pixel = -deltaValue;
					}
					else
					{
						deltaValue = deltaValue >> 1;
						pixel = deltaValue;
					}

					deltaValue = previousScanline[x] + pixel;
					*dstp = deltaValue;
					dstp++;
					x++;
					cRawBytes--;
				}

				while (nRunLength > 0)
				{
					deltaValue = previousScanline[x] + pixel;
					*dstp = deltaValue;
					dstp++;
					x++;
					nRunLength--;
				}
			}
		}

		previousScanline = currentScanline;
	}

	return (INT32)(srcp - pSrcData);
}