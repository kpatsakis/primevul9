static BOOL rfx_compose_message_header(RFX_CONTEXT* context, wStream* s)
{
	if (!Stream_EnsureRemainingCapacity(s, 12 + 10 + 12 + 13))
		return FALSE;

	rfx_write_message_sync(context, s);
	rfx_write_message_context(context, s);
	rfx_write_message_codec_versions(context, s);
	rfx_write_message_channels(context, s);
	return TRUE;
}