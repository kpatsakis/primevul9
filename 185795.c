BOOL rfx_context_reset(RFX_CONTEXT* context, UINT32 width, UINT32 height)
{
	if (!context)
		return FALSE;

	context->width = width;
	context->height = height;
	context->state = RFX_STATE_SEND_HEADERS;
	context->expectedDataBlockType = WBT_FRAME_BEGIN;
	context->frameIdx = 0;
	return TRUE;
}