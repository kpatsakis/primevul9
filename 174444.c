rdp_recv(uint8 * type)
{
	static STREAM rdp_s;
	uint16 length, pdu_type;
	uint8 rdpver;

	if ((rdp_s == NULL) || (g_next_packet >= rdp_s->end) || (g_next_packet == NULL))
	{
		rdp_s = sec_recv(&rdpver);
		if (rdp_s == NULL)
			return NULL;
		if (rdpver == 0xff)
		{
			g_next_packet = rdp_s->end;
			*type = 0;
			return rdp_s;
		}
		else if (rdpver != 3)
		{
			/* rdp5_process should move g_next_packet ok */
			rdp5_process(rdp_s);
			*type = 0;
			return rdp_s;
		}

		g_next_packet = rdp_s->p;
	}
	else
	{
		rdp_s->p = g_next_packet;
	}

	in_uint16_le(rdp_s, length);
	/* 32k packets are really 8, keepalive fix */
	if (length == 0x8000)
	{
		g_next_packet += 8;
		*type = 0;
		return rdp_s;
	}
	in_uint16_le(rdp_s, pdu_type);
	in_uint8s(rdp_s, 2);	/* userid */
	*type = pdu_type & 0xf;

#if WITH_DEBUG
	DEBUG(("RDP packet #%d, (type %x)\n", ++g_packetno, *type));
	hexdump(g_next_packet, length);
#endif /*  */

	g_next_packet += length;
	return rdp_s;
}