BYTE* freerdp_bitmap_planar_delta_encode_plane(const BYTE* inPlane, UINT32 width, UINT32 height,
                                               BYTE* outPlane)
{
	char s2c;
	INT32 delta;
	UINT32 y, x;
	BYTE* outPtr;
	const BYTE *srcPtr, *prevLinePtr;

	if (!outPlane)
	{
		if (width * height == 0)
			return NULL;

		if (!(outPlane = (BYTE*)calloc(height, width)))
			return NULL;
	}

	// first line is copied as is
	CopyMemory(outPlane, inPlane, width);
	outPtr = outPlane + width;
	srcPtr = inPlane + width;
	prevLinePtr = inPlane;

	for (y = 1; y < height; y++)
	{
		for (x = 0; x < width; x++, outPtr++, srcPtr++, prevLinePtr++)
		{
			delta = *srcPtr - *prevLinePtr;
			s2c = (delta >= 0) ? (char)delta : (char)(~((BYTE)(-delta)) + 1);
			s2c = (s2c >= 0) ? (s2c << 1) : (char)(((~((BYTE)s2c) + 1) << 1) - 1);
			*outPtr = (BYTE)s2c;
		}
	}

	return outPlane;
}