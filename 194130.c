static BYTE get_bpp_bmf(UINT32 bpp, BOOL* pValid)
{
	if (pValid)
		*pValid = TRUE;
	switch (bpp)
	{
		case 1:
			return 1;
		case 8:
			return 3;
		case 16:
			return 4;
		case 24:
			return 5;
		case 32:
			return 6;
		default:
			WLog_WARN(TAG, "Invalid color depth %" PRIu32, bpp);
			if (pValid)
				*pValid = FALSE;
			return 0;
	}
}