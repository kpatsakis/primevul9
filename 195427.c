static INLINE UINT32 freerdp_bitmap_planar_encode_rle_bytes(const BYTE* pInBuffer,
                                                            UINT32 inBufferSize, BYTE* pOutBuffer,
                                                            UINT32 outBufferSize)
{
	BYTE symbol;
	const BYTE* pInput;
	BYTE* pOutput;
	const BYTE* pBytes;
	UINT32 cRawBytes;
	UINT32 nRunLength;
	UINT32 bSymbolMatch;
	UINT32 nBytesWritten;
	UINT32 nTotalBytesWritten;
	symbol = 0;
	cRawBytes = 0;
	nRunLength = 0;
	pInput = pInBuffer;
	pOutput = pOutBuffer;
	nTotalBytesWritten = 0;

	if (!outBufferSize)
		return 0;

	do
	{
		if (!inBufferSize)
			break;

		bSymbolMatch = (symbol == *pInput) ? TRUE : FALSE;
		symbol = *pInput;
		pInput++;
		inBufferSize--;

		if (nRunLength && !bSymbolMatch)
		{
			if (nRunLength < 3)
			{
				cRawBytes += nRunLength;
				nRunLength = 0;
			}
			else
			{
				pBytes = pInput - (cRawBytes + nRunLength + 1);
				nBytesWritten = freerdp_bitmap_planar_write_rle_bytes(pBytes, cRawBytes, nRunLength,
				                                                      pOutput, outBufferSize);
				nRunLength = 0;

				if (!nBytesWritten || (nBytesWritten > outBufferSize))
					return nRunLength;

				nTotalBytesWritten += nBytesWritten;
				outBufferSize -= nBytesWritten;
				pOutput += nBytesWritten;
				cRawBytes = 0;
			}
		}

		nRunLength += bSymbolMatch;
		cRawBytes += (!bSymbolMatch) ? TRUE : FALSE;
	} while (outBufferSize);

	if (cRawBytes || nRunLength)
	{
		pBytes = pInput - (cRawBytes + nRunLength);
		nBytesWritten = freerdp_bitmap_planar_write_rle_bytes(pBytes, cRawBytes, nRunLength,
		                                                      pOutput, outBufferSize);

		if (!nBytesWritten)
			return 0;

		nTotalBytesWritten += nBytesWritten;
	}

	if (inBufferSize)
		return 0;

	return nTotalBytesWritten;
}