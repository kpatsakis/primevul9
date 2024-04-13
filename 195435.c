BOOL freerdp_bitmap_planar_compress_plane_rle(const BYTE* inPlane, UINT32 width, UINT32 height,
                                              BYTE* outPlane, UINT32* dstSize)
{
	UINT32 index;
	const BYTE* pInput;
	BYTE* pOutput;
	UINT32 outBufferSize;
	UINT32 nBytesWritten;
	UINT32 nTotalBytesWritten;

	if (!outPlane)
		return FALSE;

	index = 0;
	pInput = inPlane;
	pOutput = outPlane;
	outBufferSize = *dstSize;
	nTotalBytesWritten = 0;

	while (outBufferSize)
	{
		nBytesWritten =
		    freerdp_bitmap_planar_encode_rle_bytes(pInput, width, pOutput, outBufferSize);

		if ((!nBytesWritten) || (nBytesWritten > outBufferSize))
			return FALSE;

		outBufferSize -= nBytesWritten;
		nTotalBytesWritten += nBytesWritten;
		pOutput += nBytesWritten;
		pInput += width;
		index++;

		if (index >= height)
			break;
	}

	*dstSize = nTotalBytesWritten;
	return TRUE;
}