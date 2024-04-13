static void ttusb_dec_process_pva(struct ttusb_dec *dec, u8 *pva, int length)
{
	if (length < 8) {
		printk("%s: packet too short - discarding\n", __func__);
		return;
	}

	if (length > 8 + MAX_PVA_LENGTH) {
		printk("%s: packet too long - discarding\n", __func__);
		return;
	}

	switch (pva[2]) {

	case 0x01: {		/* VideoStream */
		int prebytes = pva[5] & 0x03;
		int postbytes = (pva[5] & 0x0c) >> 2;
		__be16 v_pes_payload_length;

		if (output_pva) {
			dec->video_filter->feed->cb.ts(pva, length, NULL, 0,
				&dec->video_filter->feed->feed.ts, NULL);
			return;
		}

		if (dec->v_pes_postbytes > 0 &&
		    dec->v_pes_postbytes == prebytes) {
			memcpy(&dec->v_pes[dec->v_pes_length],
			       &pva[12], prebytes);

			dvb_filter_pes2ts(&dec->v_pes2ts, dec->v_pes,
					  dec->v_pes_length + prebytes, 1);
		}

		if (pva[5] & 0x10) {
			dec->v_pes[7] = 0x80;
			dec->v_pes[8] = 0x05;

			dec->v_pes[9] = 0x21 | ((pva[8] & 0xc0) >> 5);
			dec->v_pes[10] = ((pva[8] & 0x3f) << 2) |
					 ((pva[9] & 0xc0) >> 6);
			dec->v_pes[11] = 0x01 |
					 ((pva[9] & 0x3f) << 2) |
					 ((pva[10] & 0x80) >> 6);
			dec->v_pes[12] = ((pva[10] & 0x7f) << 1) |
					 ((pva[11] & 0xc0) >> 7);
			dec->v_pes[13] = 0x01 | ((pva[11] & 0x7f) << 1);

			memcpy(&dec->v_pes[14], &pva[12 + prebytes],
			       length - 12 - prebytes);
			dec->v_pes_length = 14 + length - 12 - prebytes;
		} else {
			dec->v_pes[7] = 0x00;
			dec->v_pes[8] = 0x00;

			memcpy(&dec->v_pes[9], &pva[8], length - 8);
			dec->v_pes_length = 9 + length - 8;
		}

		dec->v_pes_postbytes = postbytes;

		if (dec->v_pes[9 + dec->v_pes[8]] == 0x00 &&
		    dec->v_pes[10 + dec->v_pes[8]] == 0x00 &&
		    dec->v_pes[11 + dec->v_pes[8]] == 0x01)
			dec->v_pes[6] = 0x84;
		else
			dec->v_pes[6] = 0x80;

		v_pes_payload_length = htons(dec->v_pes_length - 6 +
					     postbytes);
		memcpy(&dec->v_pes[4], &v_pes_payload_length, 2);

		if (postbytes == 0)
			dvb_filter_pes2ts(&dec->v_pes2ts, dec->v_pes,
					  dec->v_pes_length, 1);

		break;
	}

	case 0x02:		/* MainAudioStream */
		if (output_pva) {
			dec->audio_filter->feed->cb.ts(pva, length, NULL, 0,
				&dec->audio_filter->feed->feed.ts, NULL);
			return;
		}

		dvb_filter_pes2ts(&dec->a_pes2ts, &pva[8], length - 8,
				  pva[5] & 0x10);
		break;

	default:
		printk("%s: unknown PVA type: %02x.\n", __func__,
		       pva[2]);
		break;
	}
}