static void ttusb_dec_process_packet(struct ttusb_dec *dec)
{
	int i;
	u16 csum = 0;
	u16 packet_id;

	if (dec->packet_length % 2) {
		printk("%s: odd sized packet - discarding\n", __func__);
		return;
	}

	for (i = 0; i < dec->packet_length; i += 2)
		csum ^= ((dec->packet[i] << 8) + dec->packet[i + 1]);

	if (csum) {
		printk("%s: checksum failed - discarding\n", __func__);
		return;
	}

	packet_id = dec->packet[dec->packet_length - 4] << 8;
	packet_id += dec->packet[dec->packet_length - 3];

	if ((packet_id != dec->next_packet_id) && dec->next_packet_id) {
		printk("%s: warning: lost packets between %u and %u\n",
		       __func__, dec->next_packet_id - 1, packet_id);
	}

	if (packet_id == 0xffff)
		dec->next_packet_id = 0x8000;
	else
		dec->next_packet_id = packet_id + 1;

	switch (dec->packet_type) {
	case TTUSB_DEC_PACKET_PVA:
		if (dec->pva_stream_count)
			ttusb_dec_process_pva(dec, dec->packet,
					      dec->packet_payload_length);
		break;

	case TTUSB_DEC_PACKET_SECTION:
		if (dec->filter_stream_count)
			ttusb_dec_process_filter(dec, dec->packet,
						 dec->packet_payload_length);
		break;

	case TTUSB_DEC_PACKET_EMPTY:
		break;
	}
}