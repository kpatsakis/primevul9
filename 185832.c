BOOL rfx_write_message(RFX_CONTEXT* context, wStream* s, RFX_MESSAGE* message)
{
	if (context->state == RFX_STATE_SEND_HEADERS)
	{
		if (!rfx_compose_message_header(context, s))
			return FALSE;

		context->state = RFX_STATE_SEND_FRAME_DATA;
	}

	if (!rfx_write_message_frame_begin(context, s, message) ||
	    !rfx_write_message_region(context, s, message) ||
	    !rfx_write_message_tileset(context, s, message) ||
	    !rfx_write_message_frame_end(context, s, message))
	{
		return FALSE;
	}

	return TRUE;
}