rdpsnd_process(STREAM s)
{
	uint16 len;

	while (!s_check_end(s))
	{
		/* New packet */
		if (packet.size == 0)
		{
			if ((s->end - s->p) < 4)
			{
				error("RDPSND: Split at packet header. Things will go south from here...\n");
				return;
			}
			in_uint8(s, packet_opcode);
			in_uint8s(s, 1);	/* Padding */
			in_uint16_le(s, len);

			DEBUG_SOUND(("RDPSND: == Opcode %x Length: %d ==\n",
				     (int) packet_opcode, (int) len));

			packet.p = packet.data;
			packet.end = packet.data + len;
			packet.size = len;
		}
		else
		{
			len = MIN(s->end - s->p, packet.end - packet.p);

			/* Microsoft's server is so broken it's not even funny... */
			if (packet_opcode == RDPSND_WRITE)
			{
				if ((packet.p - packet.data) < 12)
					len = MIN(len, 12 - (packet.p - packet.data));
				else if ((packet.p - packet.data) == 12)
				{
					DEBUG_SOUND(("RDPSND: Eating 4 bytes of %d bytes...\n",
						     len));
					in_uint8s(s, 4);
					len -= 4;
				}
			}

			in_uint8a(s, packet.p, len);
			packet.p += len;
		}

		/* Packet fully assembled */
		if (packet.p == packet.end)
		{
			packet.p = packet.data;
			rdpsnd_process_packet(packet_opcode, &packet);
			packet.size = 0;
		}
	}
}