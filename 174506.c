rdpsnd_clear_record(void)
{
	/*
	 * Silently drop everything we have in the record buffer as
	 * we've somehow gotten a reset in regard to the server.
	 */
	record_buffer_size = 0;
}