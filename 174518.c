rdpdr_send_completion(uint32 device, uint32 id, uint32 status, uint32 result, uint8 * buffer,
		      uint32 length)
{
	STREAM s;

#ifdef WITH_SCARD
	scard_lock(SCARD_LOCK_RDPDR);
#endif
	s = channel_init(rdpdr_channel, 20 + length);
	out_uint16_le(s, RDPDR_CTYP_CORE);
	out_uint16_le(s, PAKID_CORE_DEVICE_IOCOMPLETION);
	out_uint32_le(s, device);
	out_uint32_le(s, id);
	out_uint32_le(s, status);
	out_uint32_le(s, result);
	out_uint8p(s, buffer, length);
	s_mark_end(s);
	/* JIF */
#ifdef WITH_DEBUG_RDP5
	printf("--> rdpdr_send_completion\n");
	/* hexdump(s->channel_hdr + 8, s->end - s->channel_hdr - 8); */
#endif
	channel_send(s, rdpdr_channel);
#ifdef WITH_SCARD
	scard_unlock(SCARD_LOCK_RDPDR);
#endif
}