static INLINE INT32 planar_decompress_plane_rle(const BYTE* pSrcData, UINT32 SrcSize,
                                                BYTE* pDstData, INT32 nDstStep, UINT32 nXDst,
                                                UINT32 nYDst, UINT32 nWidth, UINT32 nHeight,
                                                UINT32 nChannel, BOOL vFlip)
{
	INT32 x, y;
	UINT32 pixel;
	UINT32 cRawBytes;
	UINT32 nRunLength;
	INT32 deltaValue;
	INT32 beg, end, inc;
	BYTE controlByte;
	BYTE* currentScanline;
	BYTE* previousScanline;
	const BYTE* srcp = pSrcData;

	if ((nHeight > INT32_MAX) || (nWidth > INT32_MAX) || (nDstStep > INT32_MAX))
		return -1;

	previousScanline = NULL;

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

			if (((dstp + (cRawBytes + nRunLength)) - currentScanline) > nWidth * 4)
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
					dstp += 4;
					x++;
					cRawBytes--;
				}

				while (nRunLength > 0)
				{
					*dstp = pixel;
					dstp += 4;
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

					deltaValue = previousScanline[x * 4] + pixel;
					*dstp = deltaValue;
					dstp += 4;
					x++;
					cRawBytes--;
				}

				while (nRunLength > 0)
				{
					deltaValue = previousScanline[x * 4] + pixel;
					*dstp = deltaValue;
					dstp += 4;
					x++;
					nRunLength--;
				}
			}
		}

		previousScanline = currentScanline;
	}

	return (INT32)(srcp - pSrcData);
}