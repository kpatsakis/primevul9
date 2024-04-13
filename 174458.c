rdpsnd_send_completion(uint16 tick, uint8 packet_index)
{
	STREAM s;

	s = rdpsnd_init_packet(RDPSND_COMPLETION, 4);
	out_uint16_le(s, tick);
	out_uint8(s, packet_index);
	out_uint8(s, 0);
	s_mark_end(s);
	rdpsnd_send(s);

	DEBUG_SOUND(("RDPSND: -> RDPSND_COMPLETION(tick: %u, index: %u)\n",
		     (unsigned) tick, (unsigned) packet_index));
}