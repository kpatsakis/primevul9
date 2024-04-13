static BYTE get_bmf_bpp(UINT32 bmf, BOOL* pValid)
{
	if (pValid)
		*pValid = TRUE;
	switch (bmf)
	{
		case 1:
			return 1;
		case 3:
			return 8;
		case 4:
			return 16;
		case 5:
			return 24;
		case 6:
			return 32;
		default:
			WLog_WARN(TAG, "Invalid bmf %" PRIu32, bmf);
			if (pValid)
				*pValid = FALSE;
			return 0;
	}
}