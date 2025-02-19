static int tcos_decipher(sc_card_t *card, const u8 * crgram, size_t crgram_len, u8 * out, size_t outlen)
{
	sc_context_t *ctx;
	sc_apdu_t apdu;
	u8 rbuf[SC_MAX_APDU_BUFFER_SIZE];
	u8 sbuf[SC_MAX_APDU_BUFFER_SIZE];
	tcos_data *data;
	int tcos3, r;

	assert(card != NULL && crgram != NULL && out != NULL);
	ctx = card->ctx;
	tcos3=(card->type==SC_CARD_TYPE_TCOS_V3);
	data=(tcos_data *)card->drv_data;

	SC_FUNC_CALLED(ctx, SC_LOG_DEBUG_NORMAL);
	sc_debug(ctx, SC_LOG_DEBUG_NORMAL,
		"TCOS3:%d PKCS1:%d\n",tcos3,
		!!(data->pad_flags & SC_ALGORITHM_RSA_PAD_PKCS1));

	sc_format_apdu(card, &apdu, crgram_len>255 ? SC_APDU_CASE_4_EXT : SC_APDU_CASE_4_SHORT, 0x2A, 0x80, 0x86);
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	apdu.le = crgram_len;

	apdu.data = sbuf;
	apdu.lc = apdu.datalen = crgram_len+1;
	sbuf[0] = tcos3 ? 0x00 : ((data->pad_flags & SC_ALGORITHM_RSA_PAD_PKCS1) ? 0x81 : 0x02);
	memcpy(sbuf+1, crgram, crgram_len);

	r = sc_transmit_apdu(card, &apdu);
	SC_TEST_RET(card->ctx, SC_LOG_DEBUG_NORMAL, r, "APDU transmit failed");

	if (apdu.sw1==0x90 && apdu.sw2==0x00) {
		size_t len= (apdu.resplen>outlen) ? outlen : apdu.resplen;
		unsigned int offset=0;
		if(tcos3 && (data->pad_flags & SC_ALGORITHM_RSA_PAD_PKCS1) && apdu.resp[0]==0 && apdu.resp[1]==2){
			offset=2; while(offset<len && apdu.resp[offset]!=0) ++offset;
			offset=(offset<len-1) ? offset+1 : 0;
		}
		memcpy(out, apdu.resp+offset, len-offset);
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, len-offset);
	}
	SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, sc_check_sw(card, apdu.sw1, apdu.sw2));
}