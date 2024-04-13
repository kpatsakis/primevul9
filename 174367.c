rdpsnd_process_rec_negotiate(STREAM in)
{
	uint16 in_format_count, i;
	uint16 version;
	RD_WAVEFORMATEX *format;
	STREAM out;
	RD_BOOL device_available = False;
	int readcnt;
	int discardcnt;

	in_uint8s(in, 8);	/* initial bytes not valid from server */
	in_uint16_le(in, in_format_count);
	in_uint16_le(in, version);

	DEBUG_SOUND(("RDPSND: RDPSND_REC_NEGOTIATE(formats: %d, version: %x)\n",
		     (int) in_format_count, (unsigned) version));

	if (rdpsnd_rec_negotiated)
	{
		error("RDPSND: Extra RDPSND_REC_NEGOTIATE in the middle of a session\n");
		/* Do a complete reset of the sound state */
		rdpsnd_reset_state();
	}

	if (!current_driver)
		device_available = rdpsnd_auto_select();

	if (current_driver && !device_available && current_driver->wave_in_open
	    && current_driver->wave_in_open())
	{
		current_driver->wave_in_close();
		device_available = True;
	}

	rec_format_count = 0;
	if (s_check_rem(in, 18 * in_format_count))
	{
		for (i = 0; i < in_format_count; i++)
		{
			format = &rec_formats[rec_format_count];
			in_uint16_le(in, format->wFormatTag);
			in_uint16_le(in, format->nChannels);
			in_uint32_le(in, format->nSamplesPerSec);
			in_uint32_le(in, format->nAvgBytesPerSec);
			in_uint16_le(in, format->nBlockAlign);
			in_uint16_le(in, format->wBitsPerSample);
			in_uint16_le(in, format->cbSize);

			/* read in the buffer of unknown use */
			readcnt = format->cbSize;
			discardcnt = 0;
			if (format->cbSize > MAX_CBSIZE)
			{
				fprintf(stderr, "cbSize too large for buffer: %d\n",
					format->cbSize);
				readcnt = MAX_CBSIZE;
				discardcnt = format->cbSize - MAX_CBSIZE;
			}
			in_uint8a(in, format->cb, readcnt);
			in_uint8s(in, discardcnt);

			if (current_driver && current_driver->wave_in_format_supported
			    && current_driver->wave_in_format_supported(format))
			{
				rec_format_count++;
				if (rec_format_count == MAX_FORMATS)
					break;
			}
		}
	}

	out = rdpsnd_init_packet(RDPSND_REC_NEGOTIATE, 12 + 18 * rec_format_count);
	out_uint32_le(out, 0x00000000);	/* flags */
	out_uint32_le(out, 0xffffffff);	/* volume */
	out_uint16_le(out, rec_format_count);
	out_uint16_le(out, 1);	/* version */

	for (i = 0; i < rec_format_count; i++)
	{
		format = &rec_formats[i];
		out_uint16_le(out, format->wFormatTag);
		out_uint16_le(out, format->nChannels);
		out_uint32_le(out, format->nSamplesPerSec);
		out_uint32_le(out, format->nAvgBytesPerSec);
		out_uint16_le(out, format->nBlockAlign);
		out_uint16_le(out, format->wBitsPerSample);
		out_uint16(out, 0);	/* cbSize */
	}

	s_mark_end(out);

	DEBUG_SOUND(("RDPSND: -> RDPSND_REC_NEGOTIATE(formats: %d)\n", (int) rec_format_count));

	rdpsnd_send(out);

	rdpsnd_rec_negotiated = True;
}