BOOL rfx_compose_message(RFX_CONTEXT* context, wStream* s, const RFX_RECT* rects, int numRects,
                         BYTE* data, int width, int height, int scanline)
{
	RFX_MESSAGE* message;
	BOOL ret = TRUE;

	if (!(message = rfx_encode_message(context, rects, numRects, data, width, height, scanline)))
		return FALSE;

	ret = rfx_write_message(context, s, message);
	message->freeRects = TRUE;
	rfx_message_free(context, message);
	return ret;
}