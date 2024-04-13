rdpsnd_flush_record(void)
{
	STREAM s;
	unsigned int chunk_size;
	char *data;

	if (record_buffer_size == 0)
		return;

	assert(record_buffer_size <= sizeof(record_buffer));

	data = record_buffer;

	/*
	 * Microsoft's RDP server keeps dropping chunks, so we need to
	 * transmit everything inside one channel fragment or we risk
	 * making the rdpsnd server go out of sync with the byte stream.
	 */
	while (record_buffer_size)
	{
		if (record_buffer_size < 1596)
			chunk_size = record_buffer_size;
		else
			chunk_size = 1596;

		s = rdpsnd_init_packet(RDPSND_REC_DATA, chunk_size);
		out_uint8p(s, data, chunk_size);

		s_mark_end(s);
		rdpsnd_send(s);

		data = data + chunk_size;
		record_buffer_size -= chunk_size;

		DEBUG_SOUND(("RDPSND: -> RDPSND_REC_DATA(length: %u)\n", (unsigned) chunk_size));
	}

	record_buffer_size = 0;
}