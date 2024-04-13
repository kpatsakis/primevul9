static BYTE get_cbr2_bpp(UINT32 bpp, BOOL* pValid)
{
	if (pValid)
		*pValid = TRUE;
	switch (bpp)
	{
		case 3:
			return 8;
		case 4:
			return 16;
		case 5:
			return 24;
		case 6:
			return 32;
		default:
			WLog_WARN(TAG, "Invalid bpp %" PRIu32, bpp);
			if (pValid)
				*pValid = FALSE;
			return 0;
	}
}