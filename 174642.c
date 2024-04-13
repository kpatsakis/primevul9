static void ttusb_dec_process_urb_frame(struct ttusb_dec *dec, u8 *b,
					int length)
{
	swap_bytes(b, length);

	while (length) {
		switch (dec->packet_state) {

		case 0:
		case 1:
		case 2:
			if (*b++ == 0xaa)
				dec->packet_state++;
			else
				dec->packet_state = 0;

			length--;
			break;

		case 3:
			if (*b == 0x00) {
				dec->packet_state++;
				dec->packet_length = 0;
			} else if (*b != 0xaa) {
				dec->packet_state = 0;
			}

			b++;
			length--;
			break;

		case 4:
			dec->packet[dec->packet_length++] = *b++;

			if (dec->packet_length == 2) {
				if (dec->packet[0] == 'A' &&
				    dec->packet[1] == 'V') {
					dec->packet_type =
						TTUSB_DEC_PACKET_PVA;
					dec->packet_state++;
				} else if (dec->packet[0] == 'S') {
					dec->packet_type =
						TTUSB_DEC_PACKET_SECTION;
					dec->packet_state++;
				} else if (dec->packet[0] == 0x00) {
					dec->packet_type =
						TTUSB_DEC_PACKET_EMPTY;
					dec->packet_payload_length = 2;
					dec->packet_state = 7;
				} else {
					printk("%s: unknown packet type: %02x%02x\n",
					       __func__,
					       dec->packet[0], dec->packet[1]);
					dec->packet_state = 0;
				}
			}

			length--;
			break;

		case 5:
			dec->packet[dec->packet_length++] = *b++;

			if (dec->packet_type == TTUSB_DEC_PACKET_PVA &&
			    dec->packet_length == 8) {
				dec->packet_state++;
				dec->packet_payload_length = 8 +
					(dec->packet[6] << 8) +
					dec->packet[7];
			} else if (dec->packet_type ==
					TTUSB_DEC_PACKET_SECTION &&
				   dec->packet_length == 5) {
				dec->packet_state++;
				dec->packet_payload_length = 5 +
					((dec->packet[3] & 0x0f) << 8) +
					dec->packet[4];
			}

			length--;
			break;

		case 6: {
			int remainder = dec->packet_payload_length -
					dec->packet_length;

			if (length >= remainder) {
				memcpy(dec->packet + dec->packet_length,
				       b, remainder);
				dec->packet_length += remainder;
				b += remainder;
				length -= remainder;
				dec->packet_state++;
			} else {
				memcpy(&dec->packet[dec->packet_length],
				       b, length);
				dec->packet_length += length;
				length = 0;
			}

			break;
		}

		case 7: {
			int tail = 4;

			dec->packet[dec->packet_length++] = *b++;

			if (dec->packet_type == TTUSB_DEC_PACKET_SECTION &&
			    dec->packet_payload_length % 2)
				tail++;

			if (dec->packet_length ==
			    dec->packet_payload_length + tail) {
				ttusb_dec_process_packet(dec);
				dec->packet_state = 0;
			}

			length--;
			break;
		}

		default:
			printk("%s: illegal packet state encountered.\n",
			       __func__);
			dec->packet_state = 0;
		}
	}
}