static int coolkey_apdu_io(sc_card_t *card, int cla, int ins, int p1, int p2,
	const u8 * sendbuf, size_t sendbuflen, u8 ** recvbuf, size_t * recvbuflen,
	const u8 *nonce, size_t nonce_len)
{
	int r;
	sc_apdu_t apdu;
	u8 rbufinitbuf[COOLKEY_MAX_SIZE];
	u8 rsendbuf[COOLKEY_MAX_SIZE];
	u8 *rbuf;
	size_t rbuflen;
	int cse = 0;


	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	sc_log(card->ctx, 
		 "%02x %02x %02x %"SC_FORMAT_LEN_SIZE_T"u : %"SC_FORMAT_LEN_SIZE_T"u %"SC_FORMAT_LEN_SIZE_T"u\n",
		 ins, p1, p2, sendbuflen, card->max_send_size,
		 card->max_recv_size);

	rbuf = rbufinitbuf;
	rbuflen = sizeof(rbufinitbuf);

	/* if caller provided a buffer and length */
	if (recvbuf && *recvbuf && recvbuflen && *recvbuflen) {
		rbuf = *recvbuf;
		rbuflen = *recvbuflen;
	}

	if (sendbuf || nonce) {
		if (recvbuf) {
			cse = SC_APDU_CASE_4_SHORT;
		} else {
			cse = SC_APDU_CASE_3_SHORT;
		}
	} else {
		if (recvbuf) {
			cse = SC_APDU_CASE_2_SHORT;
		} else {
			cse = SC_APDU_CASE_1;
		}
	}

	/* append the nonce if we have it. Coolkey just blindly puts this at the end
	 * of the APDU (while adjusting lc). This converts case 1 to case 3. coolkey
	 * also always drops le in case 4 (which happens when proto = T0). nonces are
	 * never used on case 2 commands, so we can simply append the nonce to the data
	 * and we should be fine */
	if (nonce) {
		u8 *buf = rsendbuf;
		if (sendbuf) {
			sendbuflen = MIN(sendbuflen,sizeof(rsendbuf)-nonce_len);
			memcpy(rsendbuf, sendbuf, sendbuflen);
			buf += sendbuflen;
		}
		memcpy(buf, nonce, nonce_len);
		sendbuflen += nonce_len;
		sendbuf =rsendbuf;
	}

	sc_format_apdu(card, &apdu, cse, ins, p1, p2);

	apdu.lc = sendbuflen;
	apdu.datalen = sendbuflen;
	apdu.data = sendbuf;


	/* coolkey uses non-standard classes */
	apdu.cla = cla;

	if (recvbuf) {
		apdu.resp = rbuf;
		apdu.le = (rbuflen > 255) ? 255 : rbuflen;
		apdu.resplen = rbuflen;
	} else {
		 apdu.resp =  rbuf;
		 apdu.le = 0;
		 apdu.resplen = 0;
	}

	sc_log(card->ctx, 
		 "calling sc_transmit_apdu flags=%lx le=%"SC_FORMAT_LEN_SIZE_T"u, resplen=%"SC_FORMAT_LEN_SIZE_T"u, resp=%p",
		 apdu.flags, apdu.le, apdu.resplen, apdu.resp);

	/* with new adpu.c and chaining, this actually reads the whole object */
	r = sc_transmit_apdu(card, &apdu);

	sc_log(card->ctx, 
		 "result r=%d apdu.resplen=%"SC_FORMAT_LEN_SIZE_T"u sw1=%02x sw2=%02x",
		 r, apdu.resplen, apdu.sw1, apdu.sw2);

	if (r < 0) {
		sc_log(card->ctx, "Transmit failed");
		goto err;
	}
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	if (r < 0) {
		sc_log(card->ctx, "Transmit failed");
		goto err;
	}

	if (recvbuflen) {
		if (recvbuf && *recvbuf == NULL) {
			*recvbuf =  malloc(apdu.resplen);
			if (*recvbuf == NULL) {
				r = SC_ERROR_OUT_OF_MEMORY;
				goto err;
			}
			memcpy(*recvbuf, rbuf, apdu.resplen);
		}
		*recvbuflen =  apdu.resplen;
		r = *recvbuflen;
	}

err:
	LOG_FUNC_RETURN(card->ctx, r);
}