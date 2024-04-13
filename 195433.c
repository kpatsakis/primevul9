static INLINE BOOL freerdp_split_color_planes(const BYTE* data, UINT32 format, UINT32 width,
                                              UINT32 height, UINT32 scanline, BYTE* planes[4])
{
	INT32 i, j, k;
	if ((width > INT32_MAX) || (height > INT32_MAX) || (scanline > INT32_MAX))
		return FALSE;

	k = 0;

	if (scanline == 0)
		scanline = width * GetBytesPerPixel(format);

	for (i = (INT32)height - 1; i >= 0; i--)
	{
		const BYTE* pixel = &data[(INT32)scanline * i];

		for (j = 0; j < (INT32)width; j++)
		{
			const UINT32 color = ReadColor(pixel, format);
			pixel += GetBytesPerPixel(format);
			SplitColor(color, format, &planes[1][k], &planes[2][k], &planes[3][k], &planes[0][k],
			           NULL);
			k++;
		}
	}

	return TRUE;
}