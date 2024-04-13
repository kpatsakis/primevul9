cliprdr_send_native_format_announce(uint8 * formats_data, uint32 formats_data_length)
{
	DEBUG_CLIPBOARD(("cliprdr_send_native_format_announce\n"));

	cliprdr_send_packet(CLIPRDR_FORMAT_ANNOUNCE, CLIPRDR_REQUEST, formats_data,
			    formats_data_length);

	if (formats_data != last_formats)
	{
		if (last_formats)
			xfree(last_formats);

		last_formats = xmalloc(formats_data_length);
		memcpy(last_formats, formats_data, formats_data_length);
		last_formats_length = formats_data_length;
	}
}