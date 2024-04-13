static BOOL yuv_to_rgb(PresentationContext* presentation, BYTE* dest)
{
	const BYTE* pYUVPoint[3];
	H264_CONTEXT* h264 = presentation->h264;

	BYTE** ppYUVData;
	ppYUVData = h264->pYUVData;

	pYUVPoint[0] = ppYUVData[0];
	pYUVPoint[1] = ppYUVData[1];
	pYUVPoint[2] = ppYUVData[2];

	if (!yuv_context_decode(presentation->yuv, pYUVPoint, h264->iStride, PIXEL_FORMAT_BGRX32, dest,
	                        h264->width * 4))
	{
		WLog_ERR(TAG, "error in yuv_to_rgb conversion");
		return FALSE;
	}

	return TRUE;
}