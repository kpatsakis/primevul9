static int cac_rsa_op(sc_card_t *card,
					const u8 * data, size_t datalen,
					u8 * out, size_t outlen)
{
	int r;
	u8 *outp, *rbuf;
	size_t rbuflen, outplen;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);
	sc_debug(card->ctx, SC_LOG_DEBUG_NORMAL,
		 "datalen=%"SC_FORMAT_LEN_SIZE_T"u outlen=%"SC_FORMAT_LEN_SIZE_T"u\n",
		 datalen, outlen);

	outp = out;
	outplen = outlen;

	/* Not strictly necessary. This code requires the caller to have selected the correct PKI container
	 * and authenticated to that container with the verifyPin command... All of this under the reader lock.
	 * The PKCS #15 higher level driver code does all this correctly (it's the same for all cards, just
	 * different sets of APDU's that need to be called), so this call is really a little bit of paranoia */
	r = sc_lock(card);
	if (r != SC_SUCCESS)
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);


	rbuf = NULL;
	rbuflen = 0;
	for (; datalen > CAC_MAX_CHUNK_SIZE; data += CAC_MAX_CHUNK_SIZE, datalen -= CAC_MAX_CHUNK_SIZE) {
		r = cac_apdu_io(card, CAC_INS_SIGN_DECRYPT, CAC_P1_STEP,  0,
			data, CAC_MAX_CHUNK_SIZE, &rbuf, &rbuflen);
		if (r < 0) {
			break;
		}
		if (rbuflen != 0) {
			int n = MIN(rbuflen, outplen);
			memcpy(outp,rbuf, n);
			outp += n;
			outplen -= n;
		}
		free(rbuf);
		rbuf = NULL;
		rbuflen = 0;
	}
	if (r < 0) {
		goto err;
	}
	rbuf = NULL;
	rbuflen = 0;
	r = cac_apdu_io(card, CAC_INS_SIGN_DECRYPT, CAC_P1_FINAL, 0, data, datalen, &rbuf, &rbuflen);
	if (r < 0) {
		goto err;
	}
	if (rbuflen != 0) {
		int n = MIN(rbuflen, outplen);
		memcpy(outp,rbuf, n);
		/*outp += n;     unused */
		outplen -= n;
	}
	free(rbuf);
	rbuf = NULL;
	r = outlen-outplen;

err:
	sc_unlock(card);
	if (r < 0) {
		sc_mem_clear(out, outlen);
	}
	if (rbuf) {
		free(rbuf);
	}

	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_NORMAL, r);
}