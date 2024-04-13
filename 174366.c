rdpsnd_process_ping(STREAM in)
{
	uint16 tick;
	STREAM out;
	struct stream packet = *in;

	if (!s_check_rem(in, 4))
	{
		rdp_protocol_error("rdpsnd_process_training(), consume of training data from stream would overrun", &packet);
	}

	in_uint16_le(in, tick);

	DEBUG_SOUND(("RDPSND: RDPSND_PING(tick: 0x%04x)\n", (unsigned) tick));

	out = rdpsnd_init_packet(RDPSND_PING | 0x2300, 4);
	out_uint16_le(out, tick);
	out_uint16_le(out, 0);
	s_mark_end(out);
	rdpsnd_send(out);

	DEBUG_SOUND(("RDPSND: -> (tick: 0x%04x)\n", (unsigned) tick));
}