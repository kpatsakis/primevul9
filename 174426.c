rdpsnd_process_negotiate(STREAM in)
{
	uint16 in_format_count, i;
	uint8 pad;
	uint16 version;
	RD_WAVEFORMATEX *format;
	STREAM out;
	RD_BOOL device_available = False;
	int readcnt;
	int discardcnt;

	in_uint8s(in, 14);	/* initial bytes not valid from server */
	in_uint16_le(in, in_format_count);
	in_uint8(in, pad);
	in_uint16_le(in, version);
	in_uint8s(in, 1);	/* padding */

	DEBUG_SOUND(("RDPSND: RDPSND_NEGOTIATE(formats: %d, pad: 0x%02x, version: %x)\n",
		     (int) in_format_count, (unsigned) pad, (unsigned) version));

	if (rdpsnd_negotiated)
	{
		error("RDPSND: Extra RDPSND_NEGOTIATE in the middle of a session\n");
		/* Do a complete reset of the sound state */
		rdpsnd_reset_state();
	}

	if (!current_driver && g_rdpsnd)
		device_available = rdpsnd_auto_select();

	if (current_driver && !device_available && current_driver->wave_out_open())
	{
		current_driver->wave_out_close();
		device_available = True;
	}

	format_count = 0;
	if (s_check_rem(in, 18 * in_format_count))
	{
		for (i = 0; i < in_format_count; i++)
		{
			format = &formats[format_count];
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

			if (current_driver && current_driver->wave_out_format_supported(format))
			{
				format_count++;
				if (format_count == MAX_FORMATS)
					break;
			}
		}
	}

	out = rdpsnd_init_packet(RDPSND_NEGOTIATE | 0x200, 20 + 18 * format_count);

	uint32 flags = TSSNDCAPS_VOLUME;

	/* if sound is enabled, set snd caps to alive to enable
	   transmision of audio from server */
	if (g_rdpsnd)
	{
		flags |= TSSNDCAPS_ALIVE;
		flags |= RDPSND_FLAG_RECORD;
	}
	out_uint32_le(out, flags);	/* TSSNDCAPS flags */

	out_uint32(out, 0xffffffff);	/* volume */
	out_uint32(out, 0);	/* pitch */
	out_uint16(out, 0);	/* UDP port */

	out_uint16_le(out, format_count);
	out_uint8(out, 0);	/* padding */
	out_uint16_le(out, 2);	/* version */
	out_uint8(out, 0);	/* padding */

	for (i = 0; i < format_count; i++)
	{
		format = &formats[i];
		out_uint16_le(out, format->wFormatTag);
		out_uint16_le(out, format->nChannels);
		out_uint32_le(out, format->nSamplesPerSec);
		out_uint32_le(out, format->nAvgBytesPerSec);
		out_uint16_le(out, format->nBlockAlign);
		out_uint16_le(out, format->wBitsPerSample);
		out_uint16(out, 0);	/* cbSize */
	}

	s_mark_end(out);

	DEBUG_SOUND(("RDPSND: -> RDPSND_NEGOTIATE(formats: %d)\n", (int) format_count));

	rdpsnd_send(out);

	rdpsnd_negotiated = True;
}