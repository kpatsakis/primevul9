rdpsnd_record(const void *data, unsigned int size)
{
	uint32 remain, chunk;

	assert(rec_device_open);

	while (size)
	{
		remain = sizeof(record_buffer) - record_buffer_size;

		if (size >= remain)
			chunk = remain;
		else
			chunk = size;

		memcpy(record_buffer + record_buffer_size, data, chunk);

#ifdef B_ENDIAN
		if (current_driver->need_byteswap_on_be)
			rdpsnd_dsp_swapbytes(record_buffer + record_buffer_size,
					     chunk, &rec_formats[rec_current_format]);
#endif

		record_buffer_size += chunk;

		data = (const char *) data + chunk;
		size -= chunk;

		if (record_buffer_size == sizeof(record_buffer))
			rdpsnd_flush_record();
	}
}