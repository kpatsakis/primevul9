cliprdr_send_data(uint8 * data, uint32 length)
{
	DEBUG_CLIPBOARD(("cliprdr_send_data\n"));
	cliprdr_send_packet(CLIPRDR_DATA_RESPONSE, CLIPRDR_RESPONSE, data, length);
}