RFX_MESSAGE* rfx_encode_messages(RFX_CONTEXT* context, const RFX_RECT* rects, int numRects,
                                 BYTE* data, int width, int height, int scanline, int* numMessages,
                                 int maxDataSize)
{
	RFX_MESSAGE* message;
	RFX_MESSAGE* messageList;

	if (!(message = rfx_encode_message(context, rects, numRects, data, width, height, scanline)))
		return NULL;

	if (!(messageList = rfx_split_message(context, message, numMessages, maxDataSize)))
	{
		message->freeRects = TRUE;
		rfx_message_free(context, message);
		return NULL;
	}

	rfx_message_free(context, message);
	return messageList;
}