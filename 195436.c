static INLINE BOOL writeLine(BYTE** ppRgba, UINT32 DstFormat, UINT32 width, const BYTE** ppR,
                             const BYTE** ppG, const BYTE** ppB, const BYTE** ppA)
{
	UINT32 x;

	if (!ppRgba || !ppR || !ppG || !ppB)
		return FALSE;

	switch (DstFormat)
	{
	case PIXEL_FORMAT_BGRA32:
		for (x = 0; x < width; x++)
		{
			*(*ppRgba)++ = *(*ppB)++;
			*(*ppRgba)++ = *(*ppG)++;
			*(*ppRgba)++ = *(*ppR)++;
			*(*ppRgba)++ = *(*ppA)++;
		}

		return TRUE;

	case PIXEL_FORMAT_BGRX32:
		for (x = 0; x < width; x++)
		{
			*(*ppRgba)++ = *(*ppB)++;
			*(*ppRgba)++ = *(*ppG)++;
			*(*ppRgba)++ = *(*ppR)++;
			*(*ppRgba)++ = 0xFF;
		}

		return TRUE;

	default:
		if (ppA)
		{
			for (x = 0; x < width; x++)
			{
				BYTE alpha = *(*ppA)++;
				UINT32 color = FreeRDPGetColor(DstFormat, *(*ppR)++, *(*ppG)++, *(*ppB)++, alpha);
				WriteColor(*ppRgba, DstFormat, color);
				*ppRgba += GetBytesPerPixel(DstFormat);
			}
		}
		else
		{
			const BYTE alpha = 0xFF;

			for (x = 0; x < width; x++)
			{
				UINT32 color = FreeRDPGetColor(DstFormat, *(*ppR)++, *(*ppG)++, *(*ppB)++, alpha);
				WriteColor(*ppRgba, DstFormat, color);
				*ppRgba += GetBytesPerPixel(DstFormat);
			}
		}

		return TRUE;
	}
}